#ifdef WITH_GUI


#include "gui.h"

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
    //ImGui::InputFloat("Value", &value, 0.1f);
    //ImGui::InputFloat("Current value", &value, 0.000000000000000001f);
    //ImGui::Text("Current value: %f", currentValue);
    //ImGui::Text("Value: %f", value);
    //ImGui::Text("Phase: %f", phase);
    //ImGui::Text("Offset: %f", offsetMult);
    //ImGui::Checkbox("Use Phase", &usePhase);


    //if(ImGui::Button("+")) ChangeValue(10.0f);
    //if(ImGui::Button("-")) ChangeValue(-10.0f);


    ImGui::DragFloat("Value", &value, 0.5f);

#endif

    target = DrawFrameDataIndexDropdown("Target", target);


}

void DrumNode::Draw() {
    //ImGui::DragFloat("Value", &value);
    target = DrawFrameDataIndexDropdown("Target", target);

    
}



#else

#include "nodes.h"
#include "nodes_rotary_encoder.h"

uint32_t Node::DrawNodeDataNamesDropdown(const char* label, uint32_t currentDataValue) { return 0; }

void ValueNode::Draw() { }
void NoiseNode::Draw() { }
void SinNode::Draw() {}
void MultiplyNode::Draw() {}
void AddNode::Draw() {}
void ResampleNode::Draw() {}
void RotaryNode::Draw() {}
void DrumNode::Draw() {}
void SawNode::Draw() {}

#endif