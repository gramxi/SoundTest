#include "nodes.h"

Node::Node(NodeGraph* graph)
{
    this->graph = graph;
}

//Node::~Node() { }

void Node::AddInputPort(int uniquePortID)
{
    

    if(std::find(inputNodeIDs.begin(), inputNodeIDs.end(), uniquePortID) != inputNodeIDs.end()) {
        printf("Inputs already contain a port with id: %d", uniquePortID);
        return;
    }

    if(uniquePortID == id) {
        printf("Tried to add an input to the node object of same id.\n");
        return;
    }

    inputNodeIDs.push_back(uniquePortID);
}

void Node::RemoveInputPort(int portID)
{
    for (auto i = inputNodeIDs.begin(); i != inputNodeIDs.end(); i++)
    {
        if((*i) == portID) {
            inputNodeIDs.erase(i);
            printf("Removed link to %d.", portID);
            break;
        }
    }
    
}

void Node::CallInputs()
{
    graph->ExecuteNodes(&inputNodeIDs);
}

void Node::SetID(NodeID id)
{
    this->id = id;
}


/// @brief Saves id, name and inputs of the node
/// @param j Json object, parsed as reference (eg. with std::ref)
void Node::SaveDefaults(json& j)
{
    printf("-- Saving data... Node: %d \n", this->id);

    j["id"] = this->id;
    j["name"] = this->name;

    printf("--- Saved id and name\n");

    json inputs = json::array();

    vector<NodeID>* inputVector = this->GetInputs();

    for (size_t i = 0; i < inputVector->size(); i++)
    {
        NodeID input = inputVector->at(i);

        inputs.push_back(input);

        printf("--- Saving input %d \n", input);
    }

    j["inputs"] = inputs;

    printf("-- Data saved successfully.\n");
}

void Node::LoadDefaults(json j)
{
    if(j.contains("id")) {
        this->SetID(j.at("id"));
    }
    else
    {
        printf("Warning: Json data doesn't have an id for node. Data might be corrupt.");
    }

    printf("Loadning default data...\n");
    
    if(j.contains("inputs")) {
        for (size_t i = 0; i < j.at("inputs").size(); i++)
        {
            int input = j.at("inputs").at(i);


            this->AddInputPort(input);
        }
    }
}

float Node::TryGetValue(FrameDataIndex value, FrameData* data)
{
    auto search = data->find(value);

    if(search == data->end()) return 0.0f;

    return search->second;
}




NodeGraph::NodeGraph()
{
    frameData[CURRENT_VALUE] = 0;
    frameData[TIME] = 0;

    rootNode = 0;

    frameDataIndexNames[CURRENT_VALUE] = "Current Value";
    frameDataIndexNames[TIME] = "Time";
    frameDataIndexNames[FREQUENCY] = "Frequency";
    frameDataIndexNames[MATH_0] = "Math 0";
    frameDataIndexNames[MATH_1] = "Math 1";
    frameDataIndexNames[MATH_2] = "Math 2";
    frameDataIndexNames[MATH_3] = "Math 3";
    frameDataIndexNames[PHASE] = "Phase";

}

NodeGraph::~NodeGraph()
{
}

void NodeGraph::AddNode(NodePtr node) {

    if(nodes.size() >= MAX_NODE_COUNT - 1)
    {
        printf("Reached node count limit. \n");
        return; 
    }

    nodes.push_back(node);
}

float NodeGraph::Execute()
{
    //if(rootNode == -1) return 0.0f;
    
    frameData.clear();


    frameData[TIME] = currentTime;


    for (int i = 0; i < nodes.size(); i++)
    {
        if(nodes.at(i)->id == rootNode) {
            
            //printf("Executing root node. \n");

            NodePtr n = nodes.at(i);
            
            //printf("Node Pointer: %d \n", n);
            
            n->Execute(&frameData);
            
            return frameData.at(CURRENT_VALUE);
            
        }
    }
    
    return 0.0f;

}

void NodeGraph::ExecuteNodes(vector<NodeID> *nodes)
{
    for (size_t i = 0; i < nodes->size(); i++)
    {
        
        NodePtr node = at(nodes->at(i));

        if(node == nullptr) {
            //printf("Node at id %d doen't exist any more!\n", nodes->at(i));
            continue;
        }

        node->Execute(&frameData);


        //this->nodes.at((*i).first)->Execute(&nodesData);
    }

}

void NodeGraph::ExecuteNode(NodeID nodeId) {

    
    NodePtr node = at(nodeId);

    if(node == nullptr) {
        //printf("Node at id %d doen't exist any more!\n", nodes->at(i));
        return;
    }

    node->Execute(&frameData);
}

void NodeGraph::Step() {
    currentTime += 1.0f;


}

float NodeGraph::GetTime()
{
    return currentTime;
}

NodePtr NodeGraph::at(NodeID nodeID)
{
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if(nodes[i]->id == nodeID) return nodes[i];
    }

    return nullptr;
    
}

NodePtr NodeGraph::atIndex(uint32_t index)
{
    if(index >= nodes.size()) return nullptr;

    return nodes[index];
}

NodeID NodeGraph::GetNextFreeID()
{
    NodeID i = 0;

    while (i < MAX_NODE_COUNT)
    {
        bool sameIDFound = false;

        for (size_t n = 0; n < nodes.size(); n++)
        {
            if(nodes[n]->id == i) {
                sameIDFound = true;
                i++;
                break; 
            }
        }

        if(!sameIDFound) return i;
        
    }
    
    return 0;
}

void NodeGraph::RemoveNode(NodeID nodeId)
{
    for (auto n = nodes.begin(); n < nodes.end(); n++)
    {
        if((*n)->id == nodeId) 
        {
            //Don't forget to dispose the node before deleting it...
            (*n)->Dispose();
            
            nodes.erase(n);
            break;
        }

        (*n)->RemoveInputPort(nodeId);

    }


    
}

// int NodeGraph::NodeDataToNameIndex(uint32_t nodeDataValue)
// {
//     int index = 0;

//     for (auto i = frameDataIndexNames.begin(); i != frameDataIndexNames.end(); i++)
//     {
//         if((*i).first == nodeDataValue) return index;

//         index++;
//     }

//     return 0;
    
// }

vector<string> NodeGraph::GetNodeNames()
{
    vector<string> names;

    for (auto i = frameDataIndexNames.begin(); i != frameDataIndexNames.end(); i++)
    {
        names.push_back(i->second);
    }

    return names;
    
}

vector<FrameDataIndex> NodeGraph::GetFrameDataIndexValues()
{
    vector<FrameDataIndex> values;

    for (auto i = frameDataIndexNames.begin(); i != frameDataIndexNames.end(); i++)
    {
        values.push_back(i->first);
    }

    return values;

}

// uint32_t NodeGraph::NodeNameToIndex(const char *nodeDataName)
// {
//     for (auto i = frameDataIndexNames.begin(); i != frameDataIndexNames.end(); i++)
//     {
//         if(strcmp(i->second, nodeDataName) == 0) 
//         {
//             return i->first;
//         }
//     }

//     return -1;   

// }

float NodeGraph::ReadData(FrameDataIndex index)
{
    if(frameData.find(index) != frameData.end()) {
        return frameData[index];
    }

    return 0.0;
}

void NodeGraph::Dispose()
{
    
    for (size_t i = 0; i < nodes.size(); i++)
    {
        nodes[i]->Dispose();
    }
    
    nodes.clear();

    printf("Node graph disposed.\n");
}

void NodeGraph::SetRoot(NodeID node) {
    rootNode = node;
}

int NodeGraph::GetRoot()
{
    return rootNode;
}


// VALUE NODE

ValueNode::~ValueNode()
{
}

void ValueNode::Execute(FrameData* data) {

    CallInputs();

    data->insert_or_assign(target, value);
}

json ValueNode::Serialize()
{
    json j = json();

    j["value"] = value;
    j["target"] = target;

    SaveDefaults(std::ref(j));

    return j;
}

void ValueNode::ApplyData(json j)
{

    LoadDefaults(j);
    

    if(j.contains("value"))
    {
        this->value = j["value"];
    }

    if(j.contains("target")) 
    {
        this->target = j["target"];
    }

    //printf("All data applied.\n");

}


ResampleNode::ResampleNode(NodeGraph *graph) : Node(graph)
{
    name = "Resample Node";
    from = FrameDataIndex::CURRENT_VALUE;
    to = FrameDataIndex::CURRENT_VALUE;
}

ResampleNode::~ResampleNode()
{
}

void ResampleNode::Execute(FrameData *data)
{
    (*data)[to] = (*data)[from];
}

json ResampleNode::Serialize()
{
    json j = json();

    j["from"] = from;
    j["to"] = to;

    SaveDefaults(std::ref(j));
    return j;
}

void ResampleNode::ApplyData(json j)
{
    LoadDefaults(j);
    
    if(j.contains("from")) from = j["from"];
    if(j.contains("to")) to = j["to"];
}



//PRIMITIVES

SinNode::~SinNode()
{
}

void SinNode::Execute(FrameData *data)
{
    CallInputs();

    float freq = TryGetValue(FREQUENCY, data);
    
    freq = fabs(freq);

    float time = fmodf(TryGetValue(TIME, data), 44100.0f * freq);

    if(freq != lastFreq) { // frequency shift... 
        freqOffset += lastFreq * time * PI * 2.0f / 44100.0f - freq * time * PI * 2.0f / 44100.0f;
    
        freqOffset = fmodf(freqOffset, 2.0f * PI);

        lastFreq = freq;
    }

    (*data)[target] = sinf(time * PI * 2.0f / 44100.0f * freq + freqOffset);
}

json SinNode::Serialize() {
    json j = json();

    j["target"] = target;

    SaveDefaults(std::ref(j));

    return j;
}

void SinNode::ApplyData(json j) {
    LoadDefaults(j);

    if(j.contains("target")) {
        target = j["target"];
    }
}


NoiseNode::NoiseNode(NodeGraph* graph) : Node(graph) { 
    name = "Noise Node"; 
    target = FrameDataIndex::CURRENT_VALUE; 
    noise = daisysp::WhiteNoise(); 
    noise.Init();
    noise.SetSeed(42);
};

NoiseNode::~NoiseNode()
{
}

void NoiseNode::Execute(FrameData *data)
{
    CallInputs();

    (*data)[target] = noise.Process();
}

json NoiseNode::Serialize() {
    json j = json();

    j["target"] = target;

    SaveDefaults(std::ref(j));

    return j;
}

void NoiseNode::ApplyData(json j) {
    LoadDefaults(j);

    if(j.contains("target")) target = j["target"];
}


SawNode::SawNode(NodeGraph *graph) : Node(graph)
{
    name = "Saw Node";
    target = FrameDataIndex::CURRENT_VALUE;
    lastFreq = 0.0f;
    freqOffset = 0.0f;
}

SawNode::~SawNode()
{
}

void SawNode::Execute(FrameData *data)
{
    CallInputs();

    float freq = TryGetValue(FREQUENCY, data);

    freq = fabs(freq);

    float time = fmodf(TryGetValue(TIME, data), 44100.0f * freq);

    if(freq != lastFreq) {
        freqOffset += lastFreq * time - freq * time;
        lastFreq = freq;
    }

    (*data)[target] = ((time * freq + freqOffset) / 44100.0f - floorf((time * freq + freqOffset) / 44100.0f)) * 2.0f - 1.0f; 
       
}

json SawNode::Serialize()
{
    json j = json();

    j["target"] = target;

    SaveDefaults(std::ref(j));
    return j;
}

void SawNode::ApplyData(json j)
{
    LoadDefaults(j);
    
    if(j.contains("target")) target = j["target"];
}




//MATH 

MultiplyNode::MultiplyNode(NodeGraph *graph) : Node(graph)
{
    name = "Multiply Node";
    target = FrameDataIndex::CURRENT_VALUE;
    value = 1.0;
}

MultiplyNode::~MultiplyNode()
{
}

void MultiplyNode::Execute(FrameData *data)
{

    float res = value;

    vector<NodeID>* inputs = GetInputs();

    for (size_t i = 0; i < inputs->size(); i++)
    {
        GetGraph()->ExecuteNode(inputs->at(i));

        res *= GetGraph()->ReadData(input);

    }

    (*data)[target] = res;
    

}

json MultiplyNode::Serialize()
{
    json j = json();

    j["target"] = target;
    j["value"] = value;
    j["input"] = input;

    SaveDefaults(std::ref(j));

    return j;
}

void MultiplyNode::ApplyData(json j)
{
    LoadDefaults(j);

    if(j.contains("target")) target = j["target"];
    if(j.contains("value")) value = j["value"];
    if(j.contains("input")) input = j["input"];

}



AddNode::AddNode(NodeGraph *graph) : Node(graph)
{
    name = "Add Node";
    target = FrameDataIndex::CURRENT_VALUE;
    input = FrameDataIndex::CURRENT_VALUE;
    value = 0;
}

AddNode::~AddNode()
{
}

void AddNode::Execute(FrameData *data)
{
    float res = value;

    vector<NodeID>* inputs = GetInputs();

    for (size_t i = 0; i < inputs->size(); i++)
    {
        GetGraph()->ExecuteNode(inputs->at(i));

        res += GetGraph()->ReadData(input);

    }

    (*data)[target] = res;
    
}

json AddNode::Serialize()
{

    json j = json();

    j["value"] = value;
    j["target"] = target;
    j["input"] = input;

    SaveDefaults(std::ref(j));
    return j;
}

void AddNode::ApplyData(json j)
{
    LoadDefaults(j);

    if(j.contains("value")) value = j["value"];    
    if(j.contains("target")) target = j["target"];
    if(j.contains("input")) input = j["input"];

}


//DAISY 

DrumNode::DrumNode(NodeGraph *graph) : Node(graph)
{
    name = "Drum Node";
    drum = daisysp::AnalogBassDrum();
    target = FrameDataIndex::CURRENT_VALUE;
    time = 0.0f;

    drum.Init(44100.0f);
}

DrumNode::~DrumNode()
{
}

void DrumNode::Execute(FrameData *data)
{
    CallInputs();

    float freq = TryGetValue(FREQUENCY, data);

    time++;

    if(time > 44100.0f / freq ) {
        drum.Trig();
        time = 0.0f;
    }

    (*data)[target] = drum.Process();
}

json DrumNode::Serialize()
{
    json j = json();

    j["target"] = target;

    SaveDefaults(std::ref(j));
    return j;
}

void DrumNode::ApplyData(json j)
{
    LoadDefaults(j);
    
    if(j.contains("target")) target = j["target"];
}


