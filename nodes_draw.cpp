#ifdef WITH_GUI

#include "gui.h"
#include "nodes.h"

FrameDataIndex Node::DrawFrameDataIndexDropdown(const char* label, FrameDataIndex currentDataValue)
{

    
    vector<string> fdiNames = GetGraph()->GetNodeNames();
    vector<FrameDataIndex> fdiKeys = GetGraph()->GetFrameDataIndexValues();

    int currentNameIndex;

    auto iterator = find(fdiKeys.begin(), fdiKeys.end(), currentDataValue); //GetGraph()->NodeDataToNameIndex(currentDataValue);

    if(iterator != fdiKeys.end()) currentNameIndex = iterator - fdiKeys.begin();

    

    if(ImGui::BeginCombo(label, fdiNames[currentNameIndex].c_str()))
    {

        for (int i = 0; i < fdiNames.size(); i++)
        {
            
            ImGui::PushID((void*)(intptr_t)i);
            const bool item_selected = (fdiKeys[i] == currentNameIndex);
            const char* item_text = fdiNames[i].c_str();

            if (ImGui::Selectable(item_text, item_selected))
            {
                //value_changed = true;
                currentDataValue = fdiKeys[i];
            }
            if (item_selected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    //if(ImGui::Combo(label, &currentNameIndex, fdiNames.data()->c_str(), (int)fdiNames.size()))
    // {

    //     //const char* selectedName = nodeNames[currentNameIndex];

    //     return fdiKeys[currentNameIndex];
    // }

    return currentDataValue;

}


void NoiseNode::Draw()
{
    target = DrawFrameDataIndexDropdown("Target", target);
}

void ValueNode::Draw() {

    //ImGui::SetNextItemWidth(200);
    //ImGui::SliderFloat("Value", &value, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    ImGui::DragFloat("Value", &value);


    target = DrawFrameDataIndexDropdown("Target", target);
    
}

void SinNode::Draw()
{
    target = DrawFrameDataIndexDropdown("Target", target);
}

void SawNode::Draw() {
    //ImGui::DragFloat("Value", &value);
    target = DrawFrameDataIndexDropdown("Target", target);
}


void MultiplyNode::Draw() {


    //ImGui::SliderFloat("Value", &value, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    
    ImGui::DragFloat("Value", &value);


    input = DrawFrameDataIndexDropdown("Input", input);
    target = DrawFrameDataIndexDropdown("Target", target);
}

void AddNode::Draw() {
    ImGui::DragFloat("Value", &value);
    target = DrawFrameDataIndexDropdown("Target", target);
    input = DrawFrameDataIndexDropdown("Input", input);
}

void ResampleNode::Draw() {
    from = DrawFrameDataIndexDropdown("From", from);
    to = DrawFrameDataIndexDropdown("To", to);
}

void RotaryNode::Draw() {
#ifdef RASPI
    ImGui::Text("App is compiled for Raspberry Pi. Please connect the GPIOs of your pi to the rotary encoder device and restart the application!");
#else

    ImGui::DragFloat("Value", &value, 0.5f);

#endif

    target = DrawFrameDataIndexDropdown("Target", target);


}

void DrumNode::Draw() {
    //ImGui::DragFloat("Value", &value);
    target = DrawFrameDataIndexDropdown("Target", target);

    
}


#endif
