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

#define MAX_NODE_COUNT (1024)

typedef enum NodeDataValues {
    CURRENT_VALUE = 0,
    TIME = 1,
    FREQUENCY = 2,
    MATH_0 = 3,
    MATH_1 = 4,
    MATH_2 = 5,
    MATH_3 = 6,
    PHASE = 7,

} NodeDataValues;

class NodeGraph;
class Node;

typedef shared_ptr<Node> NodePtr;



class Node
{
private:
    vector<int> inputNodeIDs;
    NodeGraph* graph;
protected:
    const char* name;
public:
    int id = 0;
    int posX, posY;
    Node(NodeGraph* graph);
    ~Node();
    virtual void Execute(map<uint32_t, float>* data) { printf("Execution"); };
    virtual json Serialize() { return json(); };
    virtual void ApplyData(json json) {};
    virtual void Dispose() {}
    void AddInputPort(int uniquePortID);
    void RemoveInputPort(int portID);
    void CallInputs();
    void SetID(int id);
    const char* GetName() { return name; }
    vector<int>* GetInputs() {return &inputNodeIDs;}
    NodeGraph* GetGraph() { return graph; }
    void SaveDefaults(json& json);
    void LoadDefaults(json json);
    static float TryGetValue(uint32_t value,map<uint32_t, float>* data);

    //Gui functions
    virtual void Draw() {};
    uint32_t DrawNodeDataNamesDropdown(const char* label, uint32_t currentDataValue);
};

class NodeGraph
{
private:
    //All nodes stored in the tree
    vector<NodePtr> nodes;
    //Data modified by nodes - changes every frame
    map<uint32_t, float> nodesData;
    //Helper map for storing node names for each data value that can be changed (useful for dropdowns, etc.)
    map<uint32_t, const char*> nodeDataNames;
    //Node id of the node to start evoking the tree
    int rootNode;
    float currentTime = 0;
public:
    NodeGraph();
    ~NodeGraph();
    void AddNode(NodePtr node);
    void SetRoot(int nodeID);
    int GetRoot();
    float Execute();
    void ExecuteNodes(vector<int>* nodes);
    void ExecuteNode(int nodeId);
    void Step();
    float GetTime();
    size_t NodeCount() { return nodes.size(); }
    NodePtr at(int nodeID);
    NodePtr atIndex(uint32_t index);
    int GetNextFreeID();
    void RemoveNode(int nodeId);
    int NodeDataToNameIndex(uint32_t nodeDataValue);
    vector<const char*> GetNodeNames();
    uint32_t NodeNameToIndex(const char* nodeDataName);
    float ReadData(uint32_t nodeDataValue);
    void Dispose();
    
};


//VALUE 

class ValueNode : public Node
{
private:
    float value; 
    uint32_t target;
    
public:
    ValueNode(float value, uint32_t target, NodeGraph* graph) : value(value), target(target), Node(graph) { name = "Value Node"; };
    ~ValueNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json json) override;
};

class ResampleNode : public Node
{
private:
    uint32_t from;
    uint32_t to;
public:
    ResampleNode(NodeGraph* graph);
    ~ResampleNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;
};


//PRIMITIVES

class SinNode : public Node
{

private:
    uint32_t target;
    float lastFreq;
    float freqOffset;
    
public:
    //Takes TIME and FREQUENCY as input and writes into CURRENT_VALUE
    SinNode(NodeGraph* graph) : Node(graph) { name = "Sin Node"; target = NodeDataValues::CURRENT_VALUE; lastFreq = 0.0f; freqOffset = 0.0f;};
    ~SinNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json json) override;
};

class SawNode : public Node
{
private:
    uint32_t target;
    float lastFreq;
    float freqOffset;
public:
    SawNode(NodeGraph* graph);
    ~SawNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;
};

class NoiseNode : public Node
{
private:
    uint32_t target;
    daisysp::WhiteNoise noise;

public: 
    NoiseNode(NodeGraph* graph);
    ~NoiseNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;

};


// MATH 

class MultiplyNode : public Node 
{
private:
    uint32_t target;
    uint32_t input;
    float value;

public:
    MultiplyNode(NodeGraph* graph);
    ~MultiplyNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;

};

class AddNode : public Node
{
private:
    uint32_t target;
    uint32_t input;
    float value;
    
public:
    AddNode(NodeGraph* graph);
    ~AddNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;
};


//DAISY DRUMS

class DrumNode : public Node
{
private:
    daisysp::AnalogBassDrum drum;
    uint32_t target;
    float time;
    
public:
    DrumNode(NodeGraph* graph);
    ~DrumNode();
    void Execute(map<uint32_t, float>* data) override;
    void Draw() override;
    json Serialize() override;
    void ApplyData(json j) override;

    
};


#endif







