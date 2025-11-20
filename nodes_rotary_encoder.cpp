
#include "nodes_rotary_encoder.h"


json RotaryNode::Serialize()
{
    json j = json();

    j["target"] = target;
    j["value"] = value;
    //j["usePhase"] = usePhase;

    SaveDefaults(std::ref(j));
    
    
    return j;
}

void RotaryNode::ApplyData(json j)
{
    LoadDefaults(j);
    if(j.contains("target")) target = j["target"];
    if(j.contains("value")) {
        //currentValue = j["value"];
        value = j["value"];
    }
    //if(j.contains("usePhase")) usePhase = j["usePhase"];


    // std::cout << "Finished deserializing. Data: " << currentValue << std::endl;
    // std::cout << "Finished deserializing. Data: " << value << std::endl;

}



#ifdef RASPI

/*

             +---------+         +---------+      0
             |         |         |         |
   A         |         |         |         |
             |         |         |         |
   +---------+         +---------+         +----- 1

       +---------+         +---------+            0
       |         |         |         |
   B   |         |         |         |
       |         |         |         |
   ----+         +---------+         +---------+  1

*/

void re_decoder::_pulse(int gpio, int level, uint32_t tick)
{
   if (gpio == mygpioA) levA = level; else levB = level;

   if (gpio != lastGpio) /* debounce */
   {
      lastGpio = gpio;

      if ((gpio == mygpioA) && (level == 1))
      {
         std::cout << "Increased rotary value" << std::endl;
         if (levB) value += 0.1f;//(mycallback)(1);
      }
      else if ((gpio == mygpioB) && (level == 1))
      {
         std::cout << "Decreased rotary value" << std::endl;
         if (levA) value -= 0.1f;//(mycallback)(-1);
      }
   }
}

void re_decoder::_pulseEx(int gpio, int level, uint32_t tick, void *user)
{
   /*
      Need a static callback to link with C.
   */

   re_decoder *mySelf = (re_decoder *) user;

   mySelf->_pulse(gpio, level, tick); /* Call the instance callback. */
}

re_decoder::re_decoder(int gpioA, int gpioB)
{
   mygpioA = gpioA;
   mygpioB = gpioB;

   //mycallback = callback;

   value = 0.0f;

   levA=0;
   levB=0;

   lastGpio = -1;




}

re_decoder::re_decoder()
{
   
}

void re_decoder::re_start(re_decoder* userdata) {
   
   
   
   gpioSetMode(userdata->mygpioA, PI_INPUT);
   gpioSetMode(userdata->mygpioB, PI_INPUT);

   /* pull up is needed as encoder common is grounded */

   gpioSetPullUpDown(userdata->mygpioA, PI_PUD_UP);
   gpioSetPullUpDown(userdata->mygpioB, PI_PUD_UP);

   /* monitor encoder level changes */

   gpioSetAlertFuncEx(userdata->mygpioA, _pulseEx, userdata);
   gpioSetAlertFuncEx(userdata->mygpioB, _pulseEx, userdata);
}

void re_decoder::re_cancel(void)
{
   gpioSetAlertFuncEx(mygpioA, 0, this);
   gpioSetAlertFuncEx(mygpioB, 0, this);
}

float re_decoder::get_value()
{
   return value;
}

void re_decoder::set_value(float value)
{
   this->value = value;
}







RotaryNode::RotaryNode(NodeGraph *graph) : Node(graph)
{

    name = "Rotary Node";
    value = 0.0f;
    //currentValue = 0.0f;
    //phase = 0.0f;
    //usePhase = true;


    //initialize rotary listener

    if (gpioInitialise() < 0) { decoder = nullptr; }

    else {
    
        re_decoder rotary_decoder = re_decoder(22,23);
        
        decoder = (re_decoder*)malloc(sizeof(re_decoder));

        *decoder = rotary_decoder;

        re_decoder::re_start(decoder);

    }


}

RotaryNode::~RotaryNode()
{

}


void RotaryNode::Execute(FrameData* data)
{

    if(decoder != nullptr) value = decoder->get_value();


    // bool shouldTrigger = false; 
    
    // float delta = 0;

    // if(currentValue != value) {
    //     shouldTrigger = true;

    //     delta = value - currentValue;

    //     currentValue = value;

    // }

    (*data)[target] = value;

    //float time = fmodf(TryGetValue(TIME, data), 44100.0f);


    // if(shouldTrigger) {
    //     phase += time * delta;
    // }


    // if(usePhase) (*data)[PHASE] = phase;
}

void RotaryNode::Dispose()
{
    if(decoder != nullptr)
        decoder->re_cancel();

    free(decoder);

    printf("Rotary encoder disposed.");
}

void RotaryNode::ChangeValue(float delta)
{
    
}

#else

RotaryNode::RotaryNode(NodeGraph *graph) : Node(graph)
{
    name = "Rotary Node";
    value = 0.0f;
    //currentValue = 0.0f;
    //phase = 0.0f;
    //usePhase = true;

}

RotaryNode::~RotaryNode()
{
} 

void RotaryNode::Execute(FrameData* data)
{

    //bool shouldTrigger = false; 
    
    //float delta = 0;

    // if(currentValue != value) {
    //     shouldTrigger = true;

    //     delta = value - currentValue;

    //     currentValue = value;

    // }

    (*data)[target] = value;

    //float time = fmodf(TryGetValue(TIME, data), 44100.0f);


    // if(shouldTrigger) {
    //     phase += time * delta;
    // }

    //if(usePhase) (*data)[PHASE] = phase;
}

void RotaryNode::ChangeValue(float delta)
{
    value += delta;
}

void RotaryNode::Dispose()
{


}


#endif
