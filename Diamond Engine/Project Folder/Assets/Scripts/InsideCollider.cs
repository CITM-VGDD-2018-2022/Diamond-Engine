using System;
using DiamondEngine;

public class InsideCollider : DiamondComponent
{
    public float maxDistance = 5;
    public GameObject colliderPosition = null;
    public GameObject displayText = null;
    public GameObject displaySecondaryText = null;
    public GameObject selectButton = null;
    public GameObject hubTextController = null;
    private bool into = false;
    private bool outOf = false;

    public void Update()
    {
        if (displayText == null)
            return;

        if (IsInside() && displayText.IsEnabled() == false && (displaySecondaryText == null || displaySecondaryText.IsEnabled() == false)) 
        {
            displayText.EnableNav(true);

            if (hubTextController != null)
                hubTextController.GetComponent<HubTextController>().insideColliderTextActive = true;

            if (selectButton != null)
            {
                Navigation navComponent = selectButton.GetComponent<Navigation>();

                if (navComponent != null)
                    navComponent.Select();
            }
            into = true;
        }
        else if (!IsInside() && displayText.IsEnabled())
        {
            displayText.EnableNav(false);
            if (hubTextController != null)
                hubTextController.GetComponent<HubTextController>().insideColliderTextActive = false;
            outOf = true;
        }
        else if (!IsInside() && displaySecondaryText!=null && displaySecondaryText.IsEnabled())
        {
            displaySecondaryText.EnableNav(false);
            if (hubTextController != null)
                hubTextController.GetComponent<HubTextController>().insideColliderTextActive = false;
            outOf = true;
        }
        if (into)
        {
            Audio.PlayAudio(Core.instance.gameObject, "Play_Interaction_Circle_In");
            into = false;
        }
        else if (outOf)
        {
            Audio.PlayAudio(Core.instance.gameObject, "Play_Interaction_Circle_Out");
            outOf = false;
        }
    }

    public bool IsInside()
    {
        if (Core.instance == null || colliderPosition == null)
            return false;

        Vector3 playerPos = Core.instance.gameObject.transform.globalPosition;
        Vector3 colliderPos = colliderPosition.transform.globalPosition;
        double distance = playerPos.DistanceNoSqrt(colliderPos);

        if (distance >= -maxDistance && distance <= maxDistance)
            return true;
        else
            return false;
    }
}