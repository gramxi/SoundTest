#ifndef NODES
#define NODES

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "json.hpp"

#include "daisysp.h"

using namespace std;

using json = nlohmann::json;

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

#ifdef WITH_GUI
    #define GUI_ONLY(...) __VA_ARGS__
#else
    #define GUI_ONLY(...)
#endif

#define MAX_NODE_COUNT (1024)

typedef enum FrameDataIndex : uint32_t {
    CURRENT_VALUE = 0,
    TIME = 1,
    FREQUENCY = 2,
    MATH_0 = 3,
    MATH_1 = 4,
    MATH_2 = 5,
    MATH_3 = 6,
    PHASE = 7,

} FrameDataIndex;

typedef uint32_t NodeID;

class Node;
class NodeGraph;

typedef map<FrameDataIndex, float> FrameData;

class Node 
{
private:
    vector<NodeID> inputNodeIDs;
    NodeGraph* graph;
protected:
    const char* name;
public:
    NodeID id = 0;
    int posX, posY;
    Node(NodeGraph* graph);

    virtual void Execute(FrameData* data) { printf("Execution"); };
    virtual json Serialize() { return json(); };
    virtual void ApplyData(json json) {};
    virtual void Dispose() {};
    void AddInputPort(int uniquePortID);
    void RemoveInputPort(int portID);
    void CallInputs();
    void SetID(NodeID id);
    const char* GetName() { return name; }
    vector<NodeID>* GetInputs() {return &inputNodeIDs;}
    NodeGraph* GetGraph() { return graph; }
    void SaveDefaults(json& json);
    void LoadDefaults(json json);
    static float TryGetValue(FrameDataIndex value, FrameData* data);
    
    
    GUI_ONLY(
        virtual void Draw() = 0;
        //Gui functions
        FrameDataIndex DrawFrameDataIndexDropdown(const char* label, FrameDataIndex currentDataValue);
    )
};

typedef shared_ptr<Node> NodePtr;

class NodeGraph
{
private:
    //All nodes stored in the tree
    vector<NodePtr> nodes;
    //Data modified by nodes - changes every frame
    FrameData frameData;
    //Helper map for storing frame data names for each data index value that can be changed (useful for dropdowns, etc.)
    map<FrameDataIndex, std::string> frameDataIndexNames;
    //Node id of the node to start evoking the tree
    NodeID rootNode;
    float currentTime = 0;
public:
    NodeGraph();
    ~NodeGraph();
    void AddNode(NodePtr node);
    void SetRoot(NodeID id);
    int GetRoot();
    float Execute();
    void ExecuteNodes(vector<NodeID>* nodes);
    void ExecuteNode(NodeID nodeId);
    void Step();
    float GetTime();
    size_t NodeCount() { return nodes.size(); }
    NodePtr at(NodeID nodeID);
    NodePtr atIndex(uint32_t index);
    NodeID GetNextFreeID();
    void RemoveNode(NodeID nodeId);
    vector<string> GetNodeNames();
    vector<FrameDataIndex> GetFrameDataIndexValues();
    //FrameData NodeNameToIndex(const char* nodeDataName);
    float ReadData(FrameDataIndex index);
    void Dispose();
    
};


//VALUE 

class ValueNode : public Node
{
private:
    float value; 
    FrameDataIndex target;
    
public:
    ValueNode(float value, FrameDataIndex target, NodeGraph* graph) : value(value), target(target), Node(graph) { name = "Value Node"; };
    ~ValueNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json json) override;

    GUI_ONLY(
        void Draw() override;
    )
};

class ResampleNode : public Node
{
private:
    FrameDataIndex from;
    FrameDataIndex to;
public:
    ResampleNode(NodeGraph* graph);
    ~ResampleNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};


//PRIMITIVES

class SinNode : public Node
{

private:
    FrameDataIndex target;
    float lastFreq;
    float freqOffset;
    
public:
    //Takes TIME and FREQUENCY as input and writes into CURRENT_VALUE
    SinNode(NodeGraph* graph) : Node(graph) { name = "Sin Node"; target = FrameDataIndex::CURRENT_VALUE; lastFreq = 0.0f; freqOffset = 0.0f;};
    ~SinNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json json) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};

class SawNode : public Node
{
private:
    FrameDataIndex target;
    float lastFreq;
    float freqOffset;
public:
    SawNode(NodeGraph* graph);
    ~SawNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};

class NoiseNode : public Node
{
private:
    FrameDataIndex target;
    daisysp::WhiteNoise noise;

public: 
    NoiseNode(NodeGraph* graph);
    ~NoiseNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};


// MATH 

class MultiplyNode : public Node 
{
private:
    FrameDataIndex target;
    FrameDataIndex input;
    float value;

public:
    MultiplyNode(NodeGraph* graph);
    ~MultiplyNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};

class AddNode : public Node
{
private:
    FrameDataIndex target;
    FrameDataIndex input;
    float value;
    
public:
    AddNode(NodeGraph* graph);
    ~AddNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
};


//DAISY DRUMS

class DrumNode : public Node
{
private:
    daisysp::AnalogBassDrum drum;
    FrameDataIndex target;
    float time;
    
public:
    DrumNode(NodeGraph* graph);
    ~DrumNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    
    GUI_ONLY(
        void Draw() override;
    )
    
};


#endif







