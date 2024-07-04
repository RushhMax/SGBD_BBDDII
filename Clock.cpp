#include <iostream>
#include <vector>
#include <deque>

using namespace std;

int changeFuncInt(char func){
    if(func == 'W') return 1;
    return 0;
}

struct frame_helper{
    int idFrame;
    int refBit;
    int pinCount;
    int dirtyBit;
    bool pinned;
    deque<pair<char,int>> requerimientos;

    void actualizarDirtyBit() {
        for(int i=requerimientos.size()-1; i>0; i--){
            if(requerimientos[i].second == 1){
                requerimientos[i-1].second = 1;
            }
        }
        dirtyBit = requerimientos[0].second;
    }
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
            Frames[nFrame].requerimientos.push_back(make_pair(func, changeFuncInt(func)));
            Frames[nFrame].actualizarDirtyBit();
            if(Frames[nFrame].pinned == true) pinned = false;
            Frames[nFrame].pinned = pinned;
        }    
        void unpin(int nFrame){
            if(Frames[nFrame].pinCount > 0) { Frames[nFrame].pinCount--;}
            if(Frames[nFrame].requerimientos.size() == 0){
                Frames[nFrame].dirtyBit = 0;
            }else {
                Frames[nFrame].requerimientos.pop_front();
                if(Frames[nFrame].requerimientos.size() == 0)   Frames[nFrame].dirtyBit = 0;
                else Frames[nFrame].actualizarDirtyBit();
                Frames[nFrame].actualizarDirtyBit();
            }
        }

        void newPage(int nFrame, char func, bool pinned){ // buscar si ya existe
            Frames[nFrame].pinCount = 1;
            Frames[nFrame].refBit = 1;
            if(Frames[nFrame].pinned == true) pinned = false;
            Frames[nFrame].pinned = pinned;
            Frames[nFrame].requerimientos.push_back(make_pair(func, changeFuncInt(func)));
            Frames[nFrame].actualizarDirtyBit();
            clock_hand = getNextClock_hand();  
        }

        int getNextClock_hand() {
            int clockCont = clock_hand; 
            do{
                clock_hand++;
                clock_hand = clock_hand % n_frames; 
                if(clockCont == clock_hand)  break;
            }while(Frames[clock_hand].pinned);
            return clock_hand;
        }

        int victima(){ // puede efectuarse bucle
            for(int i=0; i<3; i++){
                int clockCont = clock_hand; 
                do{
                    //std::cout<<" Clock Hand : "<<clock_hand<<", Ref_bit : "<<Frames[clock_hand].refBit <<", dirty_bit: "<<Frames[clock_hand].dirtyBit<<", pin_count : "<<Frames[clock_hand].pinCount<<", pinned: "<<Frames[clock_hand].pinned<<endl;
                    if(!Frames[clock_hand].pinned){
                        if(Frames[clock_hand].refBit  == 0){
                            if( Frames[clock_hand].dirtyBit == 0 ) { return clock_hand;}
                            else if (i==2){return clock_hand;}
                        }else Frames[clock_hand].refBit  = 0;
                    }
                    clock_hand = getNextClock_hand();
                }while(clockCont != clock_hand);
            }
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

