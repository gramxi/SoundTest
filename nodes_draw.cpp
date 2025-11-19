#ifdef WITH_GUI


#include "gui.h"

uint32_t Node::DrawNodeDataNamesDropdown(const char* label, uint32_t currentDataValue)
{

    int currentNameIndex = GetGraph()->NodeDataToNameIndex(currentDataValue);

    vector<const char*> nodeNames = GetGraph()->GetNodeNames();

    if(ImGui::Combo(label, &currentNameIndex, nodeNames.data(), (int)nodeNames.size()))
    {
        //printf("New item selected\n");

        const char* selectedName = nodeNames[currentNameIndex];

        return GetGraph()->NodeNameToIndex(selectedName);
    }

    return currentDataValue;

}



void NoiseNode::Draw()
{
    target = DrawNodeDataNamesDropdown("Target", target);
}

void ValueNode::Draw() {

    //ImGui::SetNextItemWidth(200);
    //ImGui::SliderFloat("Value", &value, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    ImGui::DragFloat("Value", &value);


    target = DrawNodeDataNamesDropdown("Target", target);
    
}

void SinNode::Draw()
{
    target = DrawNodeDataNamesDropdown("Target", target);
}

void SawNode::Draw() {
    //ImGui::DragFloat("Value", &value);
    target = DrawNodeDataNamesDropdown("Target", target);
}


void MultiplyNode::Draw() {


    //ImGui::SliderFloat("Value", &value, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    
    ImGui::DragFloat("Value", &value);


    input = DrawNodeDataNamesDropdown("Input", input);
    target = DrawNodeDataNamesDropdown("Target", target);
}

void AddNode::Draw() {
    ImGui::DragFloat("Value", &value);
    target = DrawNodeDataNamesDropdown("Target", target);
    input = DrawNodeDataNamesDropdown("Input", input);
}

void ResampleNode::Draw() {
    from = DrawNodeDataNamesDropdown("From", from);
    to = DrawNodeDataNamesDropdown("To", to);
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

    target = DrawNodeDataNamesDropdown("Target", target);


}

void DrumNode::Draw() {
    //ImGui::DragFloat("Value", &value);
    target = DrawNodeDataNamesDropdown("Target", target);

    
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