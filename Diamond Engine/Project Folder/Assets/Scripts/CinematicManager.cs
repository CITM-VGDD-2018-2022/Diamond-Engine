using System;
using DiamondEngine;
using System.Collections.Generic;

public class CinematicManager : DiamondComponent
{
    public static CinematicManager instance;

    public GameObject gameCamera;
    public GameObject cameraPos1;

    public GameObject sequence1;
    public GameObject sequence2;
    public GameObject sequence3;
    public GameObject sequence4;
    public GameObject sequence5;
    public GameObject sequence6;
    public GameObject sequence7;
    public GameObject sequence8;
    public GameObject sequence9;
    public GameObject sequence10;
    public GameObject sequence11;


    private Vector3 initPos;
    private Quaternion initRot;
    public GameObject postCinematicDialogue;
    public GameObject cinematicDialogue;
    public bool init = false;

    private List<Sequence> listSequences = new List<Sequence>();
    public void Awake()
    {
        if (!Counter.firstRun)
        {
            StartGame(false);
            init = false;
            return;
        }

        Audio.SetState("Game_State", "Cinematic");

        instance = this;
        AddSequence(sequence1);
        AddSequence(sequence2);
        AddSequence(sequence3);
        AddSequence(sequence4);
        AddSequence(sequence5);
        AddSequence(sequence6);
        AddSequence(sequence7);
        AddSequence(sequence8);
        AddSequence(sequence9);
        AddSequence(sequence10);
        AddSequence(sequence11);

        //Maping actions
        if(listSequences.Count > 0)
        {
            listSequences[1].onStartSequence = () => 
            {
                Input.PlayHaptic(0.3f, 500);
                Audio.PlayAudio(gameObject, "Play_Razor_Appearing");
            };

            listSequences[4].onStartSequence = () =>
            {
                cinematicDialogue.Enable(true);
                gameObject.GetComponent<CinematicDialog>().StartDialog();
                Audio.PlayAudio(gameObject, "Play_Mando_Dialog");

           
                Audio.PlayAudio(gameObject, "Play_Razor_Motor");
            };

            listSequences[6].onStartSequence = () =>
            {
                Input.PlayHaptic(0.3f, 800);
                Audio.StopOneAudio(gameObject, "Play_Razor_Motor");
                Audio.PlayAudio(gameObject, "Play_Razor_Leaving");
            };
        }

        //Start first sequence
        if (listSequences.Count > 0)
        {
            listSequences[0].StartRunning();
        }

        if (gameCamera == null || cameraPos1 == null)
        {
            return;
        }

        initPos = gameCamera.transform.localPosition;
        initRot = gameCamera.transform.localRotation;
        SetAsPerspectiveCamera();

        gameCamera.transform.localPosition = cameraPos1.transform.localPosition;
        gameCamera.transform.localRotation = cameraPos1.transform.localRotation;
    }

    private void StartGame(bool withDialog)
    {
        gameCamera.GetComponent<CameraController>().startFollow = true;
        CameraManager.SetCameraOrthographic(gameCamera);
        if (withDialog)
        {
            postCinematicDialogue.Enable(true);
            postCinematicDialogue.GetChild("Button").GetComponent<Navigation>().Select();
        }
        PlayHUBMusic();
    } 
    
    private void ReturnGame()
    {
        gameCamera.GetComponent<CameraController>().startFollow = true;
        gameCamera.transform.localPosition = initPos;
        gameCamera.transform.localRotation = initRot;
        postCinematicDialogue.Enable(true);
        postCinematicDialogue.GetChild("Button").GetComponent<Navigation>().Select();
        CameraManager.SetCameraOrthographic(gameCamera);
        PlayHUBMusic();
    }

    private void PlayHUBMusic()
    {
        Audio.PlayAudio(gameObject, "Play_Cantine_Ambience");
        Audio.SetState("Game_State", "HUB");
        if (MusicSourceLocate.instance != null)
            Audio.SetSwitch(MusicSourceLocate.instance.gameObject, "Player_Health", "Healthy");
    }

    public void Update()
    {

        if (!init)
        {
            return;
        }



        if (Input.GetGamepadButton(DEControllerButton.A) == KeyState.KEY_DOWN || Input.GetGamepadButton(DEControllerButton.A) == KeyState.KEY_REPEAT)
        {

                StopAllSequences();
                ReturnGame();
                init = false;
            
        }

        foreach (Sequence sequence in listSequences)
        {
            sequence.RunSequence();
        }

    }
    private void SetAsPerspectiveCamera()
    {
        CameraManager.SetCameraPerspective(gameCamera);
        CameraManager.SetVerticalFOV(gameCamera, 25.0f);
    }

    private void ResetInitalTransform()
    {
        gameCamera.transform.localPosition = initPos;
        gameCamera.transform.localRotation = initRot;
        CameraManager.SetCameraOrthographic(gameCamera);


    }

    private void AddSequence(GameObject sequenceObject)
    {
        if (sequenceObject == null) 
            return;

        Sequence sequence = sequenceObject.GetComponent<Sequence>();

        if(sequence != null)
        {
            listSequences.Add(sequence);
        }
    }

    public void StopAllSequences()
    {
        foreach (Sequence sequence in listSequences)
        {
            sequence.StopRunning();
        }
    }

    public void OnApplicationQuit()
    {
        instance = null;
    }

    public void EndFirstSequences()
    {
        BlackFade.StartFadeIn(() => {
            StopAllSequences();
            listSequences[7].StartRunning();
            BlackFade.StartFadeOut();
            PlayHUBMusic();
        });

    }
    public void EndCinematic()
    {
        StopAllSequences();
        BlackFade.StartFadeIn(() => {
            ResetInitalTransform();
            BlackFade.StartFadeOut(() => {

                gameCamera.GetComponent<CameraController>().startFollow = true;
                postCinematicDialogue.Enable(true);
                postCinematicDialogue.GetChild("Button").GetComponent<Navigation>().Select();
            });
        });

        init = false;

    }

}