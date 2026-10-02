# What is it?
A simple include that parses GTA:SA "vehicles.ide" file contents to usable functions. The "vehicles.ide" file mostly contains car spawning related data, but some of the fields may be used to other purposes. For most people this include is useful to get a vehicle's type and group.

# How to use it?

1) Place "vehicles.ide" in your "scriptfiles/" folder

2) Include it in your script:
```
#include <omp_gta_vehicles_ide>

public OnFilterScriptInit() {
    ....
    if(!LoadVehiclesIde()) {
        print("ERROR: Failed to load vehicles.ide");
        return false;
    }
    ....
    return true;
}
```

3) Use the provided functions, for example:
```
new modelId = GetVehicleIdeModelIdByName("infernus");
// Returns 411.

new vehicleClass = GetVehicleIdeClassByModel(411);
// Returns GTA_VEHICLE_CLASS_EXECUTIVE for the Infernus.

new E_GTA_VEHICLE_IDE_TYPE:type = GetVehicleIdeTypeByModel(411);
// Returns GTA_VEHICLE_IDE_TYPE_CAR.

new gameName[32];
GetVehicleIdeGameNameByModel(411, gameName);
printf("GXT key for model 411: %s", gameName);
// GXT key for model 411: INFERNU
// Can be used to retrieve from "american.gxt" the proper name to display
```
