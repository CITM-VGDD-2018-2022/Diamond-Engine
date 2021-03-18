using System;
using DiamondEngine;
using System.Collections.Generic;




public class BoonSpawn : DiamondComponent
{
   
    public GameObject boonSpawnPosGO;
    Vector3 boonScale = new Vector3(1.0f, 1.0f, 1.0f);
    public float separation = 2.0f;
    bool firstFrame = true;
    List<GameObject> instancedPrefabs=new List<GameObject>();

    public void Update()
    {
        if (firstFrame)
        {
            firstFrame = false;

            SpawnBoons();
        }
    }

    public void SpawnBoons()
    {
        //TODO this has to be hardcoded for now (we do not have support for arrays of prefabs nor prefab drop on script editor)

        if (boonSpawnPosGO != null)
        {
            Vector3 pos = boonSpawnPosGO.transform.globalPosition;
            Vector3 spawnDir = boonSpawnPosGO.transform.GetRight();
            spawnDir = spawnDir * separation;
            Vector3 spawnPos = pos - spawnDir;
            Debug.Log("Left SpawnPos:" + spawnPos.ToString());
            instancedPrefabs.Add(InternalCalls.CreatePrefab(PrefabPathFromID("456478384"), spawnPos, boonSpawnPosGO.transform.globalRotation, boonScale));
            Debug.Log("Center SpawnPos:" + pos.ToString());
            instancedPrefabs.Add(InternalCalls.CreatePrefab(PrefabPathFromID("977900791"), pos, boonSpawnPosGO.GetComponent<Transform>().globalRotation, boonScale));
            spawnPos = pos + spawnDir;
            Debug.Log("Right SpawnPos:" + spawnPos.ToString());
            instancedPrefabs.Add(InternalCalls.CreatePrefab(PrefabPathFromID("1833518684"), spawnPos, boonSpawnPosGO.GetComponent<Transform>().globalRotation, boonScale));
        }

    }

    string PrefabPathFromID(string prefabID)
    {
        string ret= "Library/Prefabs/";
        ret = ret + prefabID + ".prefab";
        return ret;
    }

    public void DestroyAllCreatedBoons()
    {
        for (int i = instancedPrefabs.Count-1; i >=0; --i)
        {
            InternalCalls.Destroy(instancedPrefabs[i]);
            //instancedPrefabs.RemoveAt(i);
        }
        instancedPrefabs.Clear();
    }

}