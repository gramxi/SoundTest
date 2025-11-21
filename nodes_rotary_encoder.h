#ifndef NODES_ROTARY_ENCODER
#define NODES_ROTARY_ENCODER

#include "nodes.h"

#ifdef RASPI

#include <pigpio.h>
#include <stdint.h>
#include <iostream>


typedef void (*re_decoderCB_t)(int);

class re_decoder
{
   int mygpioA, mygpioB, levA, levB, lastGpio;
   
   //re_decoderCB_t mycallback;
   
   float value;
   
   void _pulse(int gpio, int level, uint32_t tick);
   
   /* Need a static callback to link with C. */
   static void _pulseEx(int gpio, int level, uint32_t tick, void *user);
   

   public:
   
   re_decoder(int gpioA, int gpioB);
   
   re_decoder();
   /*
   This function establishes a rotary encoder on gpioA and gpioB.
   
   When the encoder is turned the callback function is called.
   */
  
  void re_cancel(void);
  /*
  This function releases the resources used by the decoder.
  */
 
 static void re_start(re_decoder* userdata);
 
 
 float get_value();
 
 void set_value(float value);
};


#endif



class RotaryNode : public Node
{
    
private:
    FrameDataIndex target;
    float value;
    //float currentValue;
    //float phase;
    //bool usePhase;

    #ifdef RASPI
    re_decoder* decoder;
    #endif
    
public:
    RotaryNode(NodeGraph* graph);
    ~RotaryNode();
    void Execute(FrameData* data) override;
    json Serialize() override;
    void ApplyData(json j) override;
    void Dispose() override;
    void ChangeValue(float delta);
    
    GUI_ONLY(
        void Draw() override;
    )
};


#endif