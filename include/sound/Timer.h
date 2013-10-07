//
//  Timer.h
//  soundmath
//
//  Created by Katherine Breeden on 12/6/12.
//

#ifndef soundmath_Timer_h
#define soundmath_Timer_h

// STL
#include <ctime>
#include <vector>
#include <string>
#include <iostream>

class Timer{

public:
    
    Timer() {}  // this doesn't have to do much. 
    
    static
    double time_duration(const double init)
    {
        return (clock() - init) / CLOCKS_PER_SEC;
    }

    //void start_timer(std::string task_name);
    //void  stop_timer(const std::string completion_msg);
    
    void start_timer(std::string task_name)
    {
        m_timer.push_back(clock());
        m_task_name = task_name;
    }
    
    void stop_timer(const std::string completion_msg = std::string())
    {
        double duration = time_duration(m_timer.back());
        m_timer.pop_back();
        std::cout << m_task_name << " is done, total time: " << duration << " s. " << completion_msg << std::endl;
    }
    
private:
    std::vector<double> m_timer;
    std::string m_task_name;
};




#endif
