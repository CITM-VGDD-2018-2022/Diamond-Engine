#include "IM_ShaderImporter.h"
#include "OpenGL.h"
#include<string>
#include"Globals.h"
#include"IM_FileSystem.h"

void ShaderImporter::Import(char* buffer, int bSize, ResourceShader* res, const char* assetsPath)
{
//Get every shader typs string from the glsl
	//Buffer is now a complete shader with 2 types
	//Get a string for every shader type with find
	//Compile string
	//Save binary as type sizes + shader codes

	std::string bufferString(buffer);

	std::pair<size_t, char*> vertexShaderPair;
	size_t startByte = bufferString.find("#ifdef vertex");
	if (startByte != std::string::npos)
	{
		startByte += sizeof("#ifdef vertex");
		vertexShaderPair.first = bufferString.find("#endif", startByte)  - startByte - 1;
		vertexShaderPair.second = &buffer[startByte];
	}
	startByte = std::string::npos;

	std::pair<size_t, char*> fragmentShaderPair;
	startByte = bufferString.find("#ifdef fragment");
	if (startByte != std::string::npos)
	{
		startByte += sizeof("#ifdef fragment");
		fragmentShaderPair.first = bufferString.find("#endif", startByte) - startByte - 1;
		fragmentShaderPair.second = &buffer[startByte];
	}

	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;

	vertexShader = Compile(vertexShaderPair.second, ShaderType::SH_Vertex, vertexShaderPair.first);
	fragmentShader = Compile(fragmentShaderPair.second, ShaderType::SH_Frag, fragmentShaderPair.first);

	if (vertexShader != 0 && fragmentShader != 0)
	{
		res->shaderObjects[(int)ShaderType::SH_Vertex] = vertexShader;
		res->shaderObjects[(int)ShaderType::SH_Frag] = fragmentShader;

		res->LinkToProgram();

		char* saveBuffer = res->SaveShaderCustomFormat(vertexShaderPair.second, vertexShaderPair.first, fragmentShaderPair.second, fragmentShaderPair.first);
		
		std::string shaderFileName = SHADERS_PATH;
		shaderFileName += std::to_string(res->GetUID());
		shaderFileName += ".shdr";
		
		FileSystem::Save(shaderFileName.c_str(), saveBuffer, 8 + vertexShaderPair.first + fragmentShaderPair.first, false);

		//res->LoadShaderCustomFormat(shaderFileName.c_str());

		RELEASE_ARRAY(saveBuffer);
	}
}

/*Method to import different file shaders, not in use for now*/
//void ShaderImporter::Import(char* buffer, int bSize, ResourceShader* res, const char* assetsPath)
//{
//	//Get every shader typs string from the glsl
//
//	std::string pairShader = "";
//	char* pairBuffer = nullptr;
//
//	ShaderType type = GetAssetsObjType(assetsPath, pairShader);
//
//	int fobSize = FileSystem::LoadToBuffer(pairShader.c_str(), &pairBuffer);
//	if (fobSize == 0)
//		return;
//
//	GLuint vertexShader = 0;
//	GLuint fragmentShader = 0;
//
//	vertexShader = Compile(buffer, type, bSize);
//	fragmentShader = Compile(pairBuffer, ShaderType::SH_Frag, fobSize);
//
//	if (vertexShader != 0 && fragmentShader != 0)
//	{
//		res->shaderObjects[(int)ShaderType::SH_Vertex] = vertexShader;
//		res->shaderObjects[(int)ShaderType::SH_Frag] = fragmentShader;
//
//		res->LinkToProgram();
//
//		char* saveBuffer = res->SaveShaderCustomFormat(buffer, bSize, pairBuffer, fobSize);
//
//		std::string shaderFileName = SHADERS_PATH;
//		shaderFileName += std::to_string(res->GetUID());
//		shaderFileName += ".shdr";
//
//		FileSystem::Save(shaderFileName.c_str(), saveBuffer, 8 + bSize + fobSize, false);
//
//		//res->LoadShaderCustomFormat(shaderFileName.c_str());
//
//		RELEASE_ARRAY(saveBuffer);
//	}
//
//	//RELEASE_ARRAY(buffer);
//	if (pairBuffer == nullptr) {
//		LOG(LogType::L_ERROR, "One of the shader objects does not exist");
//	}
//	else
//		RELEASE_ARRAY(pairBuffer);
//}

GLuint ShaderImporter::Compile(char* fileBuffer, ShaderType type, const GLint size)
{
	GLuint compileShader = 0;
	compileShader = glCreateShader((type == ShaderType::SH_Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(compileShader, 1, &fileBuffer, &size);
	glCompileShader(compileShader);

	int success = 0;
	char infoLog[512];
	glGetShaderiv(compileShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(compileShader, 512, NULL, infoLog);

		if (type == ShaderType::SH_Vertex) 
		{
			LOG(LogType::L_ERROR, "Error compilating vertex shader: %s", infoLog);
		}
		else {
			LOG(LogType::L_ERROR, "Error compilating fragment shader: %s", infoLog);
		}
		glDeleteShader(compileShader);
	}

	return compileShader;
}

int ShaderImporter::GetTypeMacro(ShaderType type)
{
	int ret = 0;
	switch (type)
	{
	case SH_Vertex:
		ret = GL_VERTEX_SHADER;
		break;
	case SH_Frag:
		ret = GL_FRAGMENT_SHADER;
		break;
	case SH_Max:
		ret = 0;
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}

ShaderType ShaderImporter::GetAssetsObjType(const char* assetsPath, std::string &pairString)
{
	std::string ext(assetsPath);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "vert") 
	{
		pairString = assetsPath;
		pairString = pairString.substr(0, pairString.find_last_of('.') + 1);
		pairString += "frag";

		return ShaderType::SH_Vertex;
	}

	if (ext == "frag") 
	{
		pairString = pairString.substr(0, pairString.find_last_of('.') + 1);
		pairString += "vert";

		return ShaderType::SH_Frag;
	}

	LOG(LogType::L_ERROR, "SHADER EXTENSION NOT FOUND");
	return ShaderType::SH_Max;
}
