using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cameraControl : MonoBehaviour
{
    public float speedH = 3.0f;
    public float speedV = 3.0f;

    private float yaw = 0.0f;
    private float pitch = 0.0f;

    void FixedUpdate()
    {
        yaw += speedH * Input.GetAxis("Mouse X");
        pitch -= speedV * Input.GetAxis("Mouse Y");

        transform.eulerAngles = new Vector3(pitch, yaw, 0.0f);

        if (Input.GetKey("w"))
        {

        }
    }

}
