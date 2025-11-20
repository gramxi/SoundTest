#include "synth.h"

AudioEngine::AudioEngine()
{

    graph = NodeGraph();

    // SinNode sin = SinNode(&tree);

    // sin.SetID(0);

    // ValueNode value = ValueNode(300.0f, FREQUENCY, &tree);

    // value.SetID(1);

    // sin.AddInputPort(1);

    // tree.AddNode(make_shared<SinNode>(sin));
    // tree.AddNode(make_shared<ValueNode>(value));

    graph.SetRoot(0);
    
    stream = nullptr;
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::Start() {
    PaError error; 

    error = Pa_Initialize();

    if(error != paNoError) {
        printf("An error occured while starting audio engine.");
        return;
    }

    PaDeviceIndex devices = Pa_GetDeviceCount();

    for (int i = 0; i < devices; i++)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);

        //if()

        const char* deviceType = "Type";//deviceInfo->maxOutputChannels > 0 ? "Output" : "Input";

        printf("-- Found device: %s, Device type: %s \n", deviceInfo->name, deviceType);
    }

    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    error = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, &streamCallback, &graph);

    if(error != paNoError) {
        printf("An error occured while starting audio engine.\n");
        return;
    }

    error = Pa_StartStream(stream);

    if(error != paNoError) {
        printf("An error occured while starting audio engine.\n");
        return;
    }


}

void AudioEngine::Dispose() {
    PaError error;

    error = Pa_StopStream(stream);
    if(error != paNoError) {
        printf("An error occured while closing audio engine.\n");
        return;
    }

    error = Pa_CloseStream(stream);
    if(error != paNoError) {
        printf("An error occured while closing audio engine.\n");
        return;
    }

    error = Pa_Terminate();

    graph.Dispose();

    //disposeModifiers(streamData.head);

}

void AudioEngine::AddCreators() {
    creators.push_back(NodeCreator("Sin Node", [] (NodeGraph* graph, json json) { 
        SinNode node = SinNode(graph);


        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();

            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }

        graph->AddNode(make_shared<SinNode>(node));

        printf("Created Sin Node\n");
    }));

    creators.push_back(NodeCreator("Value Node", [] (NodeGraph* graph, json json) {
        ValueNode node = ValueNode(1.0f, (FrameDataIndex)0, graph);

        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();

            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }

        graph->AddNode(make_shared<ValueNode>(node));

        printf("Created Value Node\n");

    }));

    creators.push_back(NodeCreator("Noise Node", [] (NodeGraph* graph, json json) {
        NoiseNode node = NoiseNode(graph);

        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();

            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }

        graph->AddNode(make_shared<NoiseNode>(node));

        printf("Created Noise Node\n");
    
    }));

    creators.push_back(NodeCreator("Multiply Node", [] (NodeGraph* graph, json json){
        MultiplyNode node = MultiplyNode(graph);

        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();

            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }

        graph->AddNode(make_shared<MultiplyNode>(node));

        printf("Created Multiply Node\n");
    

    }));

    creators.push_back(NodeCreator("Add Node", [] (NodeGraph* graph, json json) { 
        AddNode node = AddNode(graph);
        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();
            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }
        graph->AddNode(make_shared<AddNode>(node));
        printf("Created Add Node");
    }));

    creators.push_back(NodeCreator("Saw Node", [] (NodeGraph* graph, json json) { 
        SawNode node = SawNode(graph);
        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();
            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }
        graph->AddNode(make_shared<SawNode>(node));
        printf("Created Saw Node");
    }));

    creators.push_back(NodeCreator("Resample Node", [] (NodeGraph* graph, json json) { 
        ResampleNode node = ResampleNode(graph);
        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();
            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }
        graph->AddNode(make_shared<ResampleNode>(node));
        printf("Created Resample Node");
    }));

    creators.push_back(NodeCreator("Rotary Node", [] (NodeGraph* graph, json json) { 
        RotaryNode node = RotaryNode(graph);
        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();
            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }
        graph->AddNode(make_shared<RotaryNode>(node));
        printf("Created Rotary Node\n");
    }));

    creators.push_back(NodeCreator("Drum Node", [] (NodeGraph* graph, json json) { 
        DrumNode node = DrumNode(graph);
        if(!json.contains("name")) {
            int id = graph->GetNextFreeID();
            node.SetID(id);
        }
        else
        {
            node.ApplyData(json);
        }
        graph->AddNode(make_shared<DrumNode>(node));
        printf("Created Drum Node");
    }));

}

void AudioEngine::LoadNodes()
{
    std::ifstream file("nodes.json");

    if(!file.is_open()) 
    {
        printf("Could not open file.\n");
        return;
    }

    json j;

    file >> j;

    if(j.contains("root")) {
        graph.SetRoot(j["root"]);
    }

    size_t size = j["nodes"].size();

    printf("Node array size: %s\n", std::to_string(size).data());

    for (size_t i = 0; i < j["nodes"].size(); i++)
    {
        json data = j["nodes"][i];

        if(data.is_null()) 
        {
            printf("Node data at index %lu is null.\n", i);
            continue;
        }

        for (size_t c = 0; c < creators.size(); c++)
        {
            string nameCreator = string(creators[c].GetName());
            string nameData = string(data["name"]);

            if(nameCreator == nameData) {
                printf("Names are equal.\n");

                creators[c].CreateNode(this->GetGraph(), data);

            }
        }
        
    }
    

}

void AudioEngine::Save()
{

    json result = json();
    

    result["nodes"] = json::array();
    result["root"] = graph.GetRoot();

    for (size_t i = 0; i < this->GetGraph()->NodeCount(); i++)
    {
        NodePtr n = this->GetGraph()->atIndex(i);
        json j = n->Serialize();

        result["nodes"].push_back(j);
        
    }
    


    std::ofstream file = std::ofstream("nodes.json");

    file << std::setw(4) << result << std::endl;

    file.close();

    printf("Data saved.\n");


}

int streamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void*userData) {



    unsigned long i;
    float* output = (float*) outputBuffer;
    float* input = (float*) inputBuffer;
    NodeGraph* graph = (NodeGraph*) userData;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    //(void) inputBuffer;

    // if((int)tree->GetTime() % 1000 == 0) {
    //     printf("Tree childs count: %d \n", (*tree).NodeCount());
    // }

    for (i = 0; i < framesPerBuffer; i++)
    {
        float value = graph->Execute();// (*graph).Execute();


        *output = value; //data->filter->filter(*input); // computeSample(data);
        
        output++;

        // *output = value; //data->filter->filter(*input); //computeSample(data);

        // output++;
        input++;

        graph->Step();

        //data->pos += 1;
    }

    return paContinue;
}

NodeCreator::NodeCreator(const char* name, function<void(NodeGraph*, json)> creator)
{
    this->name = name;
    this->creator = creator;
}

NodeCreator::~NodeCreator()
{
}
