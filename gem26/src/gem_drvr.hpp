#ifndef GEM_DRVR_HPP
#define GEM_DRVR_HPP

#include "gem_base.hpp"

class GemDrvr : public GemSys {
public:
    GemDrvr() : GemSys() {
        name = "drvr";
        
        methods["linux"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string drvName = (args.size() > 0) ? args[0]->toString() : "my_driver";
            std::string templateCode = 
                "#include <linux/module.h>\n"
                "#include <linux/kernel.h>\n"
                "#include <linux/init.h>\n\n"
                "MODULE_LICENSE(\"GPL\");\n"
                "MODULE_AUTHOR(\"Gem Language User\");\n"
                "MODULE_DESCRIPTION(\"A simple Linux device driver.\");\n"
                "MODULE_VERSION(\"0.1\");\n\n"
                "static int __init " + drvName + "_init(void) {\n"
                "    printk(KERN_INFO \"" + drvName + ": Initializing...\\n\");\n"
                "    return 0;\n"
                "}\n\n"
                "static void __exit " + drvName + "_exit(void) {\n"
                "    printk(KERN_INFO \"" + drvName + ": Exiting...\\n\");\n"
                "}\n\n"
                "module_init(" + drvName + "_init);\n"
                "module_exit(" + drvName + "_exit);\n";
            return std::make_shared<GemValue>(templateCode);
        }, true };

        methods["win11"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string drvName = (args.size() > 0) ? args[0]->toString() : "MyDriver";
            std::string templateCode = 
                "#include <ntddk.h>\n"
                "#include <wdf.h>\n\n"
                "DRIVER_INITIALIZE DriverEntry;\n"
                "EVT_WDF_DRIVER_DEVICE_ADD MyEvtDeviceAdd;\n\n"
                "NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {\n"
                "    WDF_DRIVER_CONFIG config;\n"
                "    NTSTATUS status;\n"
                "    WDF_DRIVER_CONFIG_INIT(&config, MyEvtDeviceAdd);\n"
                "    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);\n"
                "    return status;\n"
                "}\n\n"
                "NTSTATUS MyEvtDeviceAdd(_In_ WDFDRIVER Driver, _In_ PWDFDEVICE_INIT DeviceInit) {\n"
                "    UNREFERENCED_PARAMETER(Driver);\n"
                "    WDFDEVICE device;\n"
                "    return WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);\n"
                "}\n";
            return std::make_shared<GemValue>(templateCode);
        }, true };

        methods["macos"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string drvName = (args.size() > 0) ? args[0]->toString() : "MyDriver";
            std::string templateCode = 
                "#include <IOKit/IOLib.h>\n"
                "#include <IOKit/IOService.h>\n\n"
                "class com_gem_" + drvName + " : public IOService {\n"
                "    OSDeclareDefaultStructors(com_gem_" + drvName + ")\n"
                "public:\n"
                "    virtual bool init(OSDictionary *dictionary = 0) override;\n"
                "    virtual void free(void) override;\n"
                "    virtual IOService *probe(IOService *provider, SInt32 *score) override;\n"
                "    virtual bool start(IOService *provider) override;\n"
                "    virtual void stop(IOService *provider) override;\n"
                "};\n";
            return std::make_shared<GemValue>(templateCode);
        }, true };

        methods["android"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             std::string drvName = (args.size() > 0) ? args[0]->toString() : "my_hal";
             std::string templateCode = 
                "#define LOG_TAG \"" + drvName + "\"\n"
                "#include <log/log.h>\n"
                "#include <hardware/hardware.h>\n\n"
                "struct " + drvName + "_device_t {\n"
                "    struct hw_device_t common;\n"
                "    int (*set_value)(struct " + drvName + "_device_t *dev, int val);\n"
                "};\n\n"
                "static int " + drvName + "_open(const struct hw_module_t* module, const char* id, struct hw_device_t** device) {\n"
                "    ALOGI(\"Opening " + drvName + " HAL\");\n"
                "    return 0;\n"
                "}\n";
             return std::make_shared<GemValue>(templateCode);
        }, true };

        methods["build"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string target = (args.size() > 0) ? args[0]->toString() : "linux";
            return std::make_shared<GemValue>("Building for " + target + " using cross-compiler...");
        }, true };
        
        methods["deploy"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string target = (args.size() > 0) ? args[0]->toString() : "local";
            return std::make_shared<GemValue>("Deploying driver to " + target + "...");
        }, true };
    }
};

#endif
