#ifndef STANDALONE

#include "WI_TextEditor.h"
#include"IM_FileSystem.h"
#include"MO_MonoManager.h"
#include"MO_ResourceManager.h"
#include"IM_ShaderImporter.h"

W_TextEditor::W_TextEditor() : Window(), txtName(""), textType(Resource::Type::UNKNOWN) /*: texColorBuffer(-1)*/
{
	name = "Text Editor"; //No lng definition for C# :(
	txtEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	txtEditor.SetText("");
}

W_TextEditor::~W_TextEditor()
{

}

void W_TextEditor::Draw()
{

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/))
	{
		if(textType != Resource::Type::UNKNOWN)
			(textType == Resource::Type::SHADER) ? DrawShaderEditor() : DrawScriptEditor(); //Move to switch in a future

		if(!txtName.empty())
			txtEditor.Render(txtName.c_str());
	}
	ImGui::End();
}

void W_TextEditor::DrawShaderEditor() 
{
	ImGui::Dummy(ImVec2(10, 10));
	if (ImGui::Button("Save and compile shader"))
	{
		//Check for errors
		TempShader vertexShaderPair, fragmentShaderPair;
		if (ShaderImporter::CheckForErrors(txtEditor.GetText(), vertexShaderPair, fragmentShaderPair) == false)
			return;

		//Find resource
		uint uid = EngineExternal->moduleResources->GetMetaUID(EngineExternal->moduleResources->GetMetaPath(txtName.c_str()).c_str());
		Resource* res = EngineExternal->moduleResources->GetResourceFromUID(uid);

		//if(res != nullptr)

		//Save glsl
		//Save .shdr
		//Get resource pointer
		//Clean data
		//Reupload shader data to resource
	}
}
void W_TextEditor::DrawScriptEditor() 
{
	ImGui::Dummy(ImVec2(10, 10));
	if (ImGui::Button("Open Visual Studio Project"))
	{
		ShellExecute(0, 0, "Assembly-CSharp.sln", 0, 0, SW_SHOW);
		//EngineExternal->moduleMono->ReCompileCS();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save and Reload Script"))
	{
		std::string str = txtEditor.GetText();
		char* cstr = &str[0];

		FileSystem::Save(txtName.c_str(), cstr, str.length(), false);
	}

	ImGui::Dummy(ImVec2(10, 10));

	ImGui::Text("Editing script: "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), (!txtName.empty()) ? txtName.c_str() : "No script loaded");
}

void W_TextEditor::SetTextFromFile(const char* path)
{
	char* buffer = nullptr;
	textType = EngineExternal->moduleResources->GetTypeFromAssetExtension(path);
	FileSystem::LoadToBuffer(path, &buffer);

	TextEditor::LanguageDefinition lng;
	(textType == Resource::Type::SHADER) ? lng = TextEditor::LanguageDefinition::GLSL() : lng = TextEditor::LanguageDefinition::C();
	txtEditor.SetLanguageDefinition(lng);
	//std::string test = FileSystem::FileToText(path); //Can't use physFS because it's

	if (buffer != nullptr) 
	{
		txtName = path;
		txtEditor.SetText(buffer);

		RELEASE_ARRAY(buffer);
	}
	else
	{
		txtName = "";
	}
}

#endif // !STANDALONE