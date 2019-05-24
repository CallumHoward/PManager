// Parameter.hpp
// Callum Howard, 2019

struct Parameter {
    Parameter(std::string name) : name{name} {}
    std::string name;
};

struct ParameterFloat : public Parameter {
    ParameterFloat(std::string name, float* valueAddress) :
            Parameter{name}, valuePointer{valueAddress} {}
    ParameterFloat(std::string name, float *valueAddress, float min, float max) :
            Parameter{name}, valuePointer{valueAddress}, pMin{min}, pMax{max},
            min{min}, max{max} {}

    float* valuePointer;
    float min = 0.0f;  // artificial for range trimming
    float max = 1.0f;
    float pMin = 0.0f; // absolute
    float pMax = 1.0f;
};
