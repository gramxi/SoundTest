#include "gui.h"


NodeView::NodeView(const char* title, AudioEngine *engine)
{
    this->audioEngine = engine;
    this->title = title;
    this->nodePrompt = "";
    
}

NodeView::~NodeView()
{
}

void NodeView::Init()
{

    // audioEngine->AddCreators();

    // audioEngine->LoadNodes();

    InitWindow(1920, 1080, title);

    SetWindowState(FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);

    rlImGuiSetup(true);

    context = ed::CreateEditor();

    ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram] = ImColor(230,180,255,255);
    ImGui::GetStyle().FrameRounding = 12.0f;



}

void NodeView::Run()
{        
    while (!WindowShouldClose())
    {
            
            
        

        BeginDrawing();

        ClearBackground(RAYWHITE);

        rlImGuiBegin();

        bool open = true;
        ImGui::ShowDemoWindow(&open);


        DrawNodeGraph();

        //DrawNodeInspector();


        // ed::SetCurrentEditor(m_Context);
        // ed::Begin("My Editor", ImVec2(0.0, 0.0f));
        // int uniqueId = 1;
        // // Start drawing nodes.
        // ed::BeginNode(uniqueId++);
        //     ImGui::Text("Node A");
        //     ed::BeginPin(uniqueId++, ed::PinKind::Input);
        //         ImGui::Text("-> In");
        //     ed::EndPin();
        //     ImGui::SameLine();
        //     ed::BeginPin(uniqueId++, ed::PinKind::Output);
        //         ImGui::Text("Out ->");
        //     ed::EndPin();
        // ed::EndNode();
        // ed::End();
        // ed::SetCurrentEditor(nullptr);


        //drawNodes(m_Context, &m_Links, &nextLinkID);


        rlImGuiEnd();

        DrawFPS(20,20);

        EndDrawing();
        
    }
}

void NodeView::DrawNodeGraph() {




    ed::SetCurrentEditor(context);

    //auto& io = ImGui::GetIO();

    ed::Begin("Node Graph", ImVec2(0.0, 0.0f));


    NodeGraph* graph = audioEngine->GetGraph();

    //int linkId = MAX_NODE_COUNT * 4;

    int inputPortStart = MAX_NODE_COUNT;
    int outputPortStart = MAX_NODE_COUNT * 2;


    int linkIdStart = MAX_NODE_COUNT * 5;

    //draw nodes

    for (size_t i = 0; i < graph->NodeCount(); i++)
    {
        NodePtr n = graph->atIndex((uint32_t)i);

        if(n == nullptr) continue;

        ed::BeginNode(n->id + ID_OFFSET);

        ImGui::TextUnformatted(n->GetName());

        ImGui::BeginGroup();

        // input port

        ImGui::BeginGroup();

        ed::BeginPin(inputPortStart + n->id + ID_OFFSET, ed::PinKind::Input);
        {

            // Draw rounded knob
            ImGui::PushItemWidth(10.0f);

            
            ImVec2 pos = ImGui::GetCursorScreenPos();
            
            float sz = 10.0f; 
            
            ImGui::GetWindowDrawList()->AddCircle(ImVec2(pos.x + sz*0.5f, pos.y + sz*0.5f), sz*0.5f, ImGui::GetColorU32(ImVec4(1.0f, 1.0f,1.0f,1.0f)), 100, 1); 
            
            ImGui::Dummy(ImVec2(10, 10));
            
            ImGui::PopItemWidth();            
            
        }
        ed::EndPin();
        
        ImGui::EndGroup();

    
        float spacing = ed::GetNodeSize(n->id + ID_OFFSET).x;

        //Exclue a little bit from left and right:
        spacing -= 2.0f * 10.0f + 10.0f;
        
        if(n->IsActive()) {
            float time = 0.0f;// floorf(fmodf((float)GetTime(), 4.0f));
                
            ImGui::SameLine();


            ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(50.0f, 10.0f));
        
            ImGui::SameLine();
            //free(dots);

        }
        else{
            ImGui::SameLine(spacing);
        }
        
        
        //ImGui::SameLine(ed::GetNodeSize(n->id + ID_OFFSET).x - 40);

        
        //ed::GetNodeSize(n->id + ID_OFFSET);

        //output port

        ImGui::BeginGroup();

        ed::BeginPin(outputPortStart + n->id + ID_OFFSET, ed::PinKind::Output);
        {

            // Draw rounded knob

            ImGui::PushItemWidth(10.0f);
            
            ImVec2 pos = ImGui::GetCursorScreenPos();

            float sz = 10.0f; 

            ImGui::GetWindowDrawList()->AddCircle(ImVec2(pos.x + sz*0.5f, pos.y + sz*0.5f), sz*0.5f, ImGui::GetColorU32(ImVec4(1.0f, 1.0f,1.0f,1.0f)), 100, 1); 

            
            ImGui::Dummy(ImVec2(10, 10));
            
            ImGui::PopItemWidth();
        }
        ed::EndPin();

        ImGui::EndGroup();

        ImGui::EndGroup();





        ed::EndNode();


    }


    //draw node links

    for (size_t i = 0; i < graph->NodeCount(); i++)
    {
        NodePtr n = graph->atIndex((uint32_t)i);


        for (int i = 0; i < n->GetInputs()->size(); i++) {

            //printf("Link %d \n", n->GetInputs()->at(i));

            //first input, the output encoding

            int linkID =  n->id;
            linkID += MAX_NODE_COUNT * MAX_NODE_COUNT * n->GetInputs()->at(i);

            //printf("Drawing link: %d \n", linkID);

            ed::Link(linkID, outputPortStart + n->GetInputs()->at(i) + ID_OFFSET, inputPortStart + n->id + ID_OFFSET);

            //linkId++;
        }
    }
    //ed::Link(linkId + id_offset, MAX_NODE_COUNT * 2 + 1 + id_offset, MAX_NODE_COUNT + 0 + id_offset);



    if (ed::BeginCreate(ImVec4(1,1,0,1)))
    {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&outputPinId, &inputPinId))
        {
            bool allowConn = true;

            int inputPin = (int) ((uintptr_t)inputPinId);
            int outputPin = (int) ((uintptr_t)outputPinId);

            if(inputPin < inputPortStart || inputPin >= outputPortStart) {
                //printf("Input pin invalid");
                allowConn = false;
            }

            if(outputPin < outputPortStart || outputPin >= MAX_NODE_COUNT * 3) {
                //printf("Output pin invalid");
                allowConn = false;
            }
            


            //if the pin ids above are already not valid, maybe they're valid when we flip them...
            if(allowConn == false)
            {
                allowConn = true;

                int temp = inputPin;
                inputPin = outputPin;
                outputPin = temp;

                if(inputPin < inputPortStart || inputPin >= outputPortStart) {
                    //printf("Input pin invalid");
                    allowConn = false;
                }

                if(outputPin < outputPortStart || outputPin >= MAX_NODE_COUNT * 3) {
                    //printf("Output pin invalid");
                    allowConn = false;
                }

            }


            int inputNodeId = inputPin - inputPortStart - ID_OFFSET;
            int outputNodeId = outputPin - outputPortStart - ID_OFFSET;

            if(graph->at(inputNodeId) == nullptr) {
                //printf("Input node id invalid");
                allowConn = false;
            }

            if(graph->at(outputNodeId) == nullptr) {
                //printf("Output node id invalid.");
                allowConn = false;
            }


            // QueryNewLink returns true if editor want to create new link between pins.
            //
            // Link can be created only for two valid pins, it is up to you to
            // validate if connection make sense. Editor is happy to make any.
            //
            // Link always goes from input to output. User may choose to drag
            // link from output pin or input pin. This determine which pin ids
            // are valid and which are not:
            //   * input valid, output invalid - user started to drag new ling from input pin
            //   * input invalid, output valid - user started to drag new ling from output pin
            //   * input valid, output valid   - user dragged link over other pin, can be validated

            if (allowConn) // both are valid, let's accept link
            {
                // ed::AcceptNewItem() return true when user release mouse button.
                if (ed::AcceptNewItem())
                {



                    NodePtr node = graph->at(inputNodeId);

                    // Since we accepted new link, lets add one to our list of links.
                    node->AddInputPort(outputNodeId);
                    

                    printf("Created Link %d --> %d", outputNodeId, inputNodeId);


                    //*nextLinkID += 1;

                    //printf("Item accepted %d %d", *nextLinkID, m_Links->size());

                    // Draw new link.
                    //ed::Link(m_Links->back().Id, m_Links->back().InputId, m_Links->back().OutputId);
                }


                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
            else
            {
                ed::RejectNewItem(ImVec4(1,0,0,1), 2.0f);
            }
            
        }
    }
    ed::EndCreate(); // Wraps up object creation action handling.

    

    // Handle deletion action
    if (ed::BeginDelete())
    {
        // There may be many links marked for deletion, let's loop over them.
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId))
        {
            // If you agree that link can be deleted, accept deletion.
            if (ed::AcceptDeletedItem())
            {
                int inputPortID = ((int)((uintptr_t)deletedLinkId)) % (MAX_NODE_COUNT * MAX_NODE_COUNT);

                int outputPortID = (((int)(uintptr_t)deletedLinkId) - inputPortID) / (MAX_NODE_COUNT * MAX_NODE_COUNT); 

                printf("Removing link from %d to %d", outputPortID, inputPortID);




                graph->at(inputPortID)->RemoveInputPort(outputPortID);



                // // Then remove link from your data.
                // for (auto& link : *m_Links)
                // {
                //     if (link.Id == deletedLinkId)
                //     {
                //         printf("Deleting link");

                //         m_Links->erase(&link);
                //         break;
                //     }
                // }
            }

            // You may reject link deletion by calling:
            // ed::RejectDeletedItem();
        }


        //Node deletion

        ed::NodeId deletedNode;

        while (ed::QueryDeletedNode(&deletedNode))
        {

            if(ed::AcceptDeletedItem()) {

                int nodeId = (int)((uintptr_t)deletedNode) - ID_OFFSET;


                //Do nothing if trying to delete root node
                if(graph->GetRoot() != nodeId) {
                    graph->RemoveNode(nodeId);
                }

                
            }
        }
        

    }
    ed::EndDelete(); // Wrap up deletion action


    ed::End();

    DrawNodeInspector();


    ed::SetCurrentEditor(nullptr);

    if(IsKeyPressed(KEY_SPACE)) 
    {
        showCreateWindow = !showCreateWindow;
        if(showCreateWindow) {
            ImGui::SetNextWindowPos(ImGui::GetMousePos()); 

            nodePrompt.clear();

        }

    }

    if(showCreateWindow) {

        
        DrawCreatorWindow();

    }

}

void NodeView::DrawCreatorWindow() {

    NodeGraph* graph = audioEngine->GetGraph();

    ImGui::SetNextWindowSize(ImVec2(400, 200));
    ImGui::Begin("Create Node");

    if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
        ImGui::SetKeyboardFocusHere(0);

    ImGui::InputText("Input Node Name", (char*)nodePrompt.c_str(),nodePrompt.capacity() + 1);


    for (size_t c = 0; c < audioEngine->GetNodeCreators()->size(); c++)
    {
        if(TextFindIndex(TextToLower(audioEngine->GetNodeCreators()->at(c).GetName()),(char*) nodePrompt.c_str()) != -1) {
            if(ImGui::Button(audioEngine->GetNodeCreators()->at(c).GetName()))
            {
                printf("Creating node...");
                audioEngine->GetNodeCreators()->at(c).CreateNode(graph, json());
                showCreateWindow = false;
            }
        }     
    }
    
    

    ImGui::End();

}

void NodeView::DrawNodeInspector()
{
    ImGui::Begin("Node Inspector", nullptr, ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar())
    {
        if(ImGui::MenuItem("Tools/Show Step Count"))
        {
            printf("Current time (stepcount): %f\n", this->audioEngine->GetGraph()->GetTime());
        }


        ImGui::EndMenuBar();
    }

    std::vector<ed::NodeId> selectedNodes;

    selectedNodes.resize(ed::GetSelectedObjectCount());


    int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));

    selectedNodes.resize(nodeCount);

    if(nodeCount == 1)
    {

        ed::NodeId node = selectedNodes[0]; 

        ImGui::Text("Selected node: ");

        ImGui::SameLine();

        ImGui::TextUnformatted(std::to_string((int)(uintptr_t)node).c_str());
    
        int nodeID = (int)(uintptr_t)node - ID_OFFSET;


        if(this->audioEngine->GetGraph()->GetRoot() != nodeID) {
            if(ImGui::Button("Set as Root")) {
                this->audioEngine->GetGraph()->SetRoot(nodeID);
            }
        }
        else {
            ImGui::Text("Node is currently root node.");
        }

        ImGui::Separator();


        this->audioEngine->GetGraph()->at(nodeID)->Draw();


    }


    ImGui::End();
}

void NodeView::Dispose()
{
    ed::DestroyEditor(context);

    context = nullptr;
    
    rlImGuiShutdown();

    CloseWindow();

    //audioEngine->Save();
}
