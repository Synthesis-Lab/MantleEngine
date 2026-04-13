#ifndef MANTLE_CORE_MODULE_H
#define MANTLE_CORE_MODULE_H

class Module {
public:
    virtual ~Module() = default;

    virtual bool Initialize() = 0;
    virtual void Update(float delta_time) = 0;
    virtual void Shutdown() = 0;
    virtual const char* GetName() const = 0;
};

#endif // MANTLE_CORE_MODULE_H

