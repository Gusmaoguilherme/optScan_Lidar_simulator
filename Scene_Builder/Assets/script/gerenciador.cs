using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

//Manage the application outputs
public class gerenciador : MonoBehaviour
{
    public string destDirectory = @".\";
    private Dictionary<string, int> dataIndex = new Dictionary<string, int>();
 


    public string[] FormatValues(string[] values)
    {
        string[] newValues = new string[values.Length];
        for(int i = 0; i<values.Length; i++)
        { 
            newValues[i] = values[i].Replace(',', '.');
        }
        
        return newValues;
    }

    void CreateCameraFile(string folderPath)
    {

        var foundObjects = FindObjectsOfType<Camera>();
        Debug.Log("qtd de cameras: " + foundObjects.Length);

        foreach (var item in foundObjects)
        {
            if (item.isActiveAndEnabled == false)
                continue;

            string[] strValues = {
                
                item.transform.position.x.ToString(),
                item.transform.position.y.ToString(),
                (-item.transform.position).z.ToString(),
                item.transform.right.x.ToString(),
                item.transform.right.y.ToString(),
                (-item.transform.right.z).ToString(),
                item.transform.up.x.ToString(),
                item.transform.up.y.ToString(),
                (-item.transform.up.z).ToString(),
                item.transform.forward.x.ToString(),
                item.transform.forward.y.ToString(),
                (-item.transform.forward.z).ToString()
            };

            string[] values = FormatValues(strValues);

            using (StreamWriter file = new StreamWriter(folderPath + "\\" + "movUnity.txt",true))
            {
                file.WriteLine("{0}, {1}, {2}\t{3}, {4}, {5}\t{6}, {7}, {8}\t{9}, {10}, {11}", values);
            }
        }
        
    }


    void CreateBatch(string folderPath, string folderName)
    {
        using (StreamWriter file = new StreamWriter(folderPath + "\\" + folderName + ".bat"))
        {
            file.Write("cd ..\noptScan.exe --configure UnityExport/configUnity.txt");
        }
        using (StreamWriter file = new StreamWriter(folderPath + "\\" + folderName + "Cam.bat"))
        {
            file.Write("cd ..\noptScan.exe --configure UnityExport/configUnityCam.txt");
        }
    }


    [ContextMenu("infoObjs")]
    void InfoObjs()
    {
        var foundObjects = FindObjectsOfType<MeshFilter>();
        if (foundObjects.Length == 0)

        {
            Debug.Log("erro");
            return;
        }

        
        
        foreach (var item in foundObjects)
        {
            Debug.Log(item.name + ": " + item.mesh.vertexCount + " vertex\n" + item.mesh.subMeshCount + " submeshes\n");
           
        }
        return;
    }

    
    
    [ContextMenu("GerenciadorObjs")]
    void FindData()
    {
        int i = 0;
        string folder = "UnityExport";
        Directory.CreateDirectory(destDirectory + "\\" + folder);
        string folderPath = destDirectory + "\\" + folder;
        
        //File.Create(folderPath + "\\configTeste.txt");
        //string filePath = folderPath + "\\configTeste.txt";
        //string folderPath = @"C:\Users\leogiuris\Documents\_optScan_v3_jun2019\testeLeonardo";
        

        Debug.Log(folderPath + "\\configUnity.txt");

        using (StreamWriter file = new StreamWriter(folderPath + "\\configUnity.txt"))
        {
            file.Flush();
        }

        using (StreamWriter file = new StreamWriter(folderPath + "\\configUnityCam.txt"))
        {
            file.Flush();
        }

        using (StreamWriter file = new StreamWriter(folderPath + "\\movUnity.txt"))
        {
            file.Flush();
        }
        var foundObjects = FindObjectsOfType<data>();

        

        foreach (var item in foundObjects)
        {
            string[] values;
            float angle;
            Vector3 axis;

            Transform t = item.GetComponent<Transform>();
            GameObject go = item.gameObject;
            MeshFilter mf = item.GetComponent<MeshFilter>();
            if (!mf)
            {
                mf = item.GetComponentInChildren<MeshFilter>();
            }

            go.name = go.name.Replace(' ', '_');
            

            item.FileName = item.name;
            if (!dataIndex.ContainsKey(mf.sharedMesh.name))
            {
                dataIndex.Add(mf.sharedMesh.name, i);
                i++;
            }
            //item.id = dataIndex[mf.sharedMesh.name];

            //string[] strValues = { t.rotation.w.ToString(), t.rotation.x.ToString(), t.rotation.y.ToString(), t.rotation.z.ToString(), t.position.x.ToString(), t.position.y.ToString(), (t.position.z*(-1)).ToString() };
            //values = FormatValues(strValues);
            t.rotation.ToAngleAxis(out angle, out axis);
            
            Debug.Log(t.rotation.ToString());

            string[] strValues = { axis.x.ToString(), axis.y.ToString(), (-axis.z).ToString(), (-angle).ToString(), t.position.x.ToString(), t.position.y.ToString(), (t.position.z * (-1)).ToString() };
            // string[] strValues = { axis.x.ToString(), axis.y.ToString(), axis.z.ToString(), angle.ToString(), t.position.x.ToString(), t.position.y.ToString(), (t.position.z * (-1)).ToString() };
            values = FormatValues(strValues);

            ObjExporterGer.DoExport(go, true);
            

            //string str = ("--mesh " + folder + "/" + item.FileName + ".obj" + " --rotation " + values[0] + "," + values[1] + "," + values[2] + "," + values[3] + " --position " + values[4] + "," + values[5] + "," + values[6] + " --id " + item.id + " ").ToString();
            string str = string.Format("--mesh {0}/{1}.obj --rotation {2},{3},{4},{5} --position {6},{7},{8} --id {9} ", folder, item.FileName, values[0], values[1], values[2], values[3], values[4], values[5], values[6], item.id);
            
            using (StreamWriter file = new StreamWriter(folderPath + "\\configUnity.txt", true))
            {
                file.Write(str);
            }
            using (StreamWriter file = new StreamWriter(folderPath + "\\configUnityCam.txt", true))
            {
                file.Write(str);
            }

        } //end foreach


        using (StreamWriter file = new StreamWriter(folderPath + "\\configUnity.txt", true))
        {
            file.Write("--outputPath output/ --camera " + folder + "/movUnity.txt --optParam id,color");
        }
        using (StreamWriter file = new StreamWriter(folderPath + "\\configUnityCam.txt", true))
        {
            file.Write("--outputPath output/ --camera " + folder + "/movUnity.txt --optParam id,color");
        }
        

        CreateCameraFile(folderPath);
        CreateBatch(folderPath, folder);
        Debug.Log("Deu bom");

        //using (System.Diagnostics.Process p = new System.Diagnostics.Process())
        //{
        //    //p.StartInfo.UseShellExecute = false;
        //    p.StartInfo.FileName = folderPath + "\\UnityExport.bat";
        //    //p.StartInfo.CreateNoWindow = false;

        //    p.Start();
        //    p.WaitForExit();

        //}

        //System.Diagnostics.Process.Start(folderPath + "\\UnityExport.bat");
    }

}
