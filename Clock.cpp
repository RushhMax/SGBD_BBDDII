#include <iostream>
#include <vector>

using namespace std;


struct frame_helper{
    int idFrame;
    int pinCount;
    int refBit;
    int dirtyBit;
    bool pinned;

    frame_helper(int _idFrame, int _dirtyBit, int _pinCount, bool _pinned) : idFrame(_idFrame), dirtyBit(_dirtyBit) , pinCount(_pinCount), refBit(0), pinned(_pinned) {}
};

class Clock {
    private:
        std::vector<frame_helper> Frames;
        int n_frames;
        int clock_hand;

    public:

        Clock(int nFrames) : n_frames(nFrames){
            for(int i=0; i<nFrames; i++){ Frames.push_back(frame_helper(i, 0, 0, 0));}
            clock_hand = 0;
        }

        void pin(int nFrame, char func, bool pinned){
            Frames[nFrame].refBit = 1;
            Frames[nFrame].pinCount++;
            if(Frames[nFrame].dirtyBit == 0) {
                if(func == 'W') Frames[nFrame].dirtyBit = 1;
            } 
            if(Frames[nFrame].pinned == true) pinned = false;
            Frames[nFrame].pinned = pinned;
        }    
        void unpin(int nFrame){
            if(Frames[nFrame].pinCount == 0) return;
            //if(Frames[nFrame].pinCount == -1) Frames[nFrame].pinCount = 0;
            Frames[nFrame].pinCount--;
        }

        void newPage(int nFrame, char func, bool pinned){ // buscar si ya existe
            Frames[nFrame].pinCount = 1;
            Frames[nFrame].refBit = 1;
            if(Frames[nFrame].pinned == true) pinned = false;
            Frames[nFrame].pinned = pinned;
            if(Frames[nFrame].dirtyBit == 1 ) func = Frames[nFrame].dirtyBit = 1;
            if(func == 'W') Frames[nFrame].dirtyBit = 1;
            else Frames[nFrame].dirtyBit = 0;
            clock_hand = getNextClock_hand();  
        }

        int getNextClock_hand() {
            //cout<<" CLOCK AC> "<<clock_hand<<endl;
            int clockCont = clock_hand; 
            do{
                clock_hand++;
                clock_hand = clock_hand % n_frames; 
                if(clockCont == clock_hand)  break;
            }while(Frames[clock_hand].pinned == true);
            //cout<<" CLOCK AC> "<<clock_hand<<endl;
            return clock_hand;
        }

        int victima(){ // puede efectuarse bucle
            for(int i=0; i<3; i++){
                int clockCont = clock_hand; // 4 4  
                //clock_hand = getNextClock_hand();
                do{
                    //std::cout<<" Clock Hand : "<<clock_hand<<", Ref_bit : "<<Frames[clock_hand].refBit <<", dirty_bit: "<<Frames[clock_hand].dirtyBit<<", pin_count : "<<Frames[clock_hand].pinCount<<", pinned: "<<Frames[clock_hand].pinned<<endl;
                    if(!Frames[clock_hand].pinned){
                        if(Frames[clock_hand].refBit  == 0){
                            if( Frames[clock_hand].dirtyBit == 0 ) {
                                Frames[clock_hand].pinCount = 1; 
                                return clock_hand; 
                            }
                            else if (i==2){    
                                int choice;
                                cout<<" Desea liberar el frame "<<Frames[clock_hand].idFrame+1<<" ? (Y = 1 | N = 0)  ";cin>> choice;
                                if(choice == 1) return clock_hand;
                            }
                        }
                        else Frames[clock_hand].refBit  = 0;
                    }
                    clock_hand = getNextClock_hand();
                }while(clockCont != clock_hand);
            }
            //clock_hand--;
            return -1;
        }

        void printClock(){
            cout<<" CLOCK >  \n";
            int clockCont = clock_hand; // 4 4           
            do{
                std::cout<<" Clock Hand : "<<clock_hand<<", Ref_bit : "<<Frames[clock_hand].refBit <<", dirty_bit: "<<Frames[clock_hand].dirtyBit<<", pin_count : "<<Frames[clock_hand].pinCount<<", pinned: "<<Frames[clock_hand].pinned<<endl;
                clock_hand++;
                clock_hand = clock_hand % n_frames;  
            }while(clockCont != clock_hand);
        }
};

