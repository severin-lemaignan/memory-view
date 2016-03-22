#ifndef MEMORY_NETWORK
#define MEMORY_NETWORK

#include <Eigen/Dense>
#include <map>
#include <set>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <functional>

typedef Eigen::MatrixXd MemoryMatrix;
typedef Eigen::VectorXd MemoryVector;


typedef std::function<void(std::chrono::duration<long int, std::micro>,
                           const MemoryVector&)> LoggingFunction;

class MemoryNetwork
{

public:

    MemoryNetwork(size_t size,
                  LoggingFunction logging_function = nullptr, // user-defined callback used to store activation history
                  double Dg = 0.2,     // activation decay (per ms)
                  double Lg = 0.01,    // learning rate (per ms)
                  double Eg = 0.6,     // external influence
                  double Ig = 0.3,     // internal influence
                  double Amax = 1.0,   // maximum activation
                  double Amin = -0.2,  // minimum activation
                  double Arest = -0.1, // rest activation
                  double Winit = 0.0); // initial weights


    void activate_unit(size_t id, 
                    double level = 1.0, 
                    std::chrono::milliseconds duration = std::chrono::milliseconds(200));

    void activate_unit(const std::string& name, 
                    double level = 1.0, 
                    std::chrono::milliseconds duration = std::chrono::milliseconds(200));

    std::vector<std::string> units_names() const {return _units_names;}
    size_t unit_id(const std::string& name) const;
    void units_names(const std::set<std::string>& names);

    MemoryVector activations() const {return _activations;}
    MemoryMatrix weights() const {return _weights;}

    size_t size() const {return _activations.size();}
    int frequency() const {return _frequency;}

    /** Slow down the memory update mechanism (typically, for debugging) up to
     * 'freq' updated per seconds
     *
     * The update frequency is set to max_freq.  Calling slowdown with
     * max_freq=0 removes any previously set limit.
     */
    void max_frequency(float freq) {_max_freq = freq;}

    void set_parameter(const std::string& name, double value);
    double get_parameter(const std::string& name) const;

    void start();
    void stop();
    bool isrunning() const {return _is_running;}

    double Dg;
    double Lg;
    double Eg;
    double Ig;
    double Amax;
    double Amin;
    double Arest;
    double Winit;

private:
    MemoryVector rest_activations; // constant

    std::vector<std::string> _units_names;

    MemoryVector external_activations;
    MemoryVector external_activations_decay;
    MemoryVector internal_activations;
    MemoryVector net_activations;
    MemoryVector _activations;
    MemoryMatrix _weights;

    LoggingFunction _log_activation;

    std::random_device rd;
    std::default_random_engine gen;

    void compute_internal_activations();

    void run();
    void step();

    std::thread _network_thread;

    bool _is_running = false;

    void printout();

    int _frequency = 0;
    float _max_freq = 0.f;
    int _steps_since_last_frequency_update = 0;

    std::chrono::system_clock::time_point _start_time;
    std::chrono::system_clock::time_point _last_timestamp;
    std::chrono::system_clock::time_point _last_freq_computation;
};


#endif
