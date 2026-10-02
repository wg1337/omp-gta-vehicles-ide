#if defined _omp_gta_vehicles_ide_included
    #endinput
#endif
#define _omp_gta_vehicles_ide_included

#define GTA_VEHICLES_IDE_MAX_VEHICLES (300)
#define GTA_VEHICLES_IDE_NAME_LENGTH (32)
#define GTA_VEHICLES_IDE_TYPE_LENGTH (12)
#define GTA_VEHICLES_IDE_HANDLING_ID_LENGTH (24)
#define GTA_VEHICLES_IDE_GAME_NAME_LENGTH (32)
#define GTA_VEHICLES_IDE_ANIMS_LENGTH (24)
#define GTA_VEHICLES_IDE_MAX_LINE_LENGTH (512)
#define GTA_VEHICLES_IDE_MAX_FIELD_LENGTH (64)
#define GTA_VEHICLES_IDE_FILE "vehicles.ide"

#define GTA_VEHICLE_CLASS_NORMAL (0x0001)
#define GTA_VEHICLE_CLASS_POOR_FAMILY (0x0002)
#define GTA_VEHICLE_CLASS_RICH_FAMILY (0x0004)
#define GTA_VEHICLE_CLASS_EXECUTIVE (0x0008)
#define GTA_VEHICLE_CLASS_WORKER (0x0010)
#define GTA_VEHICLE_CLASS_BIG (0x0020)
#define GTA_VEHICLE_CLASS_TAXI (0x0040)
#define GTA_VEHICLE_CLASS_MOPED (0x0080)
#define GTA_VEHICLE_CLASS_MOTORBIKE (0x0100)
#define GTA_VEHICLE_CLASS_LEISURE_BOAT (0x0200)
#define GTA_VEHICLE_CLASS_WORKER_BOAT (0x0400)
#define GTA_VEHICLE_CLASS_BICYCLE (0x0800)

enum E_GTA_VEHICLE_IDE_TYPE {
    GTA_VEHICLE_IDE_TYPE_UNKNOWN = 0,
    GTA_VEHICLE_IDE_TYPE_CAR,
    GTA_VEHICLE_IDE_TYPE_MONSTER_TRUCK,
    GTA_VEHICLE_IDE_TYPE_QUAD,
    GTA_VEHICLE_IDE_TYPE_HELI,
    GTA_VEHICLE_IDE_TYPE_PLANE,
    GTA_VEHICLE_IDE_TYPE_BOAT,
    GTA_VEHICLE_IDE_TYPE_TRAIN,
    GTA_VEHICLE_IDE_TYPE_BIKE,
    GTA_VEHICLE_IDE_TYPE_BMX,
    GTA_VEHICLE_IDE_TYPE_TRAILER
};

enum E_VEHICLE_IDE_DATA {
    GTA_VEHICLE_IDE_MODEL_ID,
    GTA_VEHICLE_IDE_NAME[GTA_VEHICLES_IDE_NAME_LENGTH],
    GTA_VEHICLE_IDE_TXD_NAME[GTA_VEHICLES_IDE_NAME_LENGTH],
    GTA_VEHICLE_IDE_TYPE_NAME[GTA_VEHICLES_IDE_TYPE_LENGTH],
    E_GTA_VEHICLE_IDE_TYPE:GTA_VEHICLE_IDE_TYPE,
    GTA_VEHICLE_IDE_HANDLING_ID[GTA_VEHICLES_IDE_HANDLING_ID_LENGTH],
    GTA_VEHICLE_IDE_GAME_NAME[GTA_VEHICLES_IDE_GAME_NAME_LENGTH],
    GTA_VEHICLE_IDE_ANIMS[GTA_VEHICLES_IDE_ANIMS_LENGTH],
    GTA_VEHICLE_IDE_CLASS,
    GTA_VEHICLE_IDE_FREQUENCY,
    GTA_VEHICLE_IDE_FLAGS,
    GTA_VEHICLE_IDE_COMPONENT_RULES,
    bool:GTA_VEHICLE_IDE_HAS_EXTRA_DATA,
    GTA_VEHICLE_IDE_WHEEL_MODEL_ID,
    Float:GTA_VEHICLE_IDE_FRONT_WHEEL_SCALE,
    Float:GTA_VEHICLE_IDE_REAR_WHEEL_SCALE,
    GTA_VEHICLE_IDE_WHEEL_UPGRADE_CLASS
};

static gVehicleIde[GTA_VEHICLES_IDE_MAX_VEHICLES][E_VEHICLE_IDE_DATA];
static gVehicleIdeCount;
static bool:gVehicleIdeLoaded;

static stock bool:VehicleIde_CopyTrimmedRange(const source[], start, end, destination[], destinationSize) {
    while(start < end && (source[start] == ' ' || source[start] == '\t')) {
        start++;
    }

    while(end > start && (source[end - 1] == ' ' || source[end - 1] == '\t' || source[end - 1] == '\r' || source[end - 1] == '\n')) {
        end--;
    }

    new length = end - start;

    if(length <= 0)
        return false;

    if(length >= destinationSize)
        return false;

    for(new i = 0; i < length; i++) {
        destination[i] = source[start + i];
    }

    destination[length] = '\0';
    return true;
}

static stock bool:VehicleIde_GetFirstToken(const source[], output[], outputSize) {
    new start = 0;
    new length = strlen(source);

    while(start < length && (source[start] == ' ' || source[start] == '\t')) {
        start++;
    }

    new end = start;

    while(end < length && source[end] != ' ' && source[end] != '\t') {
        end++;
    }

    return VehicleIde_CopyTrimmedRange(source, start, end, output, outputSize);
}

static stock bool:VehicleIde_GetCommaField(const line[], fieldIndex, output[], outputSize) {
    new currentField = 0;
    new start = 0;
    new length = strlen(line);

    for(new i = 0; i <= length; i++) {
        if(line[i] == ',' || line[i] == '\0') {
            if(currentField == fieldIndex) {
                return VehicleIde_CopyTrimmedRange(line, start, i, output, outputSize);
            }

            currentField++;
            start = i + 1;
        }
    }
    return false;
}

static stock bool:VehicleIde_IsCommentOrEmpty(const line[]) {
    new length = strlen(line);

    for (new i = 0; i < length; i++) {
        if(line[i] != ' ' && line[i] != '\t' && line[i] != '\r' && line[i] != '\n') {
            return line[i] == '#';
        }
    }
    return true;
}

static stock VehicleIde_HexValue(character) {
    if(character >= '0' && character <= '9')
        return character - '0';

    if(character >= 'a' && character <= 'f')
        return character - 'a' + 10;

    if(character >= 'A' && character <= 'F')
        return character - 'A' + 10;

    return -1;
}

static stock VehicleIde_ParseHex(const value[]) {
    new result = 0;
    new i = 0;

    if(value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
        i = 2;

    for(; value[i] != '\0'; i++) {
        new digit = VehicleIde_HexValue(value[i]);

        if(digit == -1)
            break;

        result = (result << 4) | digit;
    }
    return result;
}

static stock E_GTA_VEHICLE_IDE_TYPE:VehicleIde_GetType(const typeName[]) {
    if (!strcmp(typeName, "car", true))
        return GTA_VEHICLE_IDE_TYPE_CAR;
    if (!strcmp(typeName, "mtruck", true))
        return GTA_VEHICLE_IDE_TYPE_MONSTER_TRUCK;
    if (!strcmp(typeName, "quad", true))
        return GTA_VEHICLE_IDE_TYPE_QUAD;
    if (!strcmp(typeName, "heli", true))
        return GTA_VEHICLE_IDE_TYPE_HELI;
    if (!strcmp(typeName, "plane", true))
        return GTA_VEHICLE_IDE_TYPE_PLANE;
    if (!strcmp(typeName, "boat", true))
        return GTA_VEHICLE_IDE_TYPE_BOAT;
    if (!strcmp(typeName, "train", true))
        return GTA_VEHICLE_IDE_TYPE_TRAIN;
    if (!strcmp(typeName, "bike", true))
        return GTA_VEHICLE_IDE_TYPE_BIKE;
    if (!strcmp(typeName, "bmx", true))
        return GTA_VEHICLE_IDE_TYPE_BMX;
    if (!strcmp(typeName, "trailer", true))
        return GTA_VEHICLE_IDE_TYPE_TRAILER;

    return GTA_VEHICLE_IDE_TYPE_UNKNOWN;
}

static stock VehicleIde_GetClass(const className[]) {
    if (!strcmp(className, "normal", true))
        return GTA_VEHICLE_CLASS_NORMAL;

    if (!strcmp(className, "poorfamily", true))
        return GTA_VEHICLE_CLASS_POOR_FAMILY;

    if (!strcmp(className, "richfamily", true))
        return GTA_VEHICLE_CLASS_RICH_FAMILY;

    if (!strcmp(className, "executive", true))
        return GTA_VEHICLE_CLASS_EXECUTIVE;

    if (!strcmp(className, "worker", true))
        return GTA_VEHICLE_CLASS_WORKER;

    if (!strcmp(className, "big", true))
        return GTA_VEHICLE_CLASS_BIG;

    if (!strcmp(className, "taxi", true))
        return GTA_VEHICLE_CLASS_TAXI;

    if (!strcmp(className, "moped", true))
        return GTA_VEHICLE_CLASS_MOPED;

    if (!strcmp(className, "motorbike", true))
        return GTA_VEHICLE_CLASS_MOTORBIKE;

    if (!strcmp(className, "leisureboat", true))
        return GTA_VEHICLE_CLASS_LEISURE_BOAT;

    if (!strcmp(className, "workerboat", true))
        return GTA_VEHICLE_CLASS_WORKER_BOAT;

    if (!strcmp(className, "bicycle", true))
        return GTA_VEHICLE_CLASS_BICYCLE;

    /*
    * Return something for unknown classes
    */
    return 0;
}

static stock VehicleIde_GetIndex(modelId) {
    for(new i = 0; i < gVehicleIdeCount; i++) {
        if(gVehicleIde[i][GTA_VEHICLE_IDE_MODEL_ID] == modelId)
            return i;
    }
    return -1;
}

static stock bool:VehicleIde_ContainsWhitespace(const text[]) {
    for(new i = 0; text[i] != '\0'; i++) {
        if(text[i] == ' ' || text[i] == '\t')
            return true;
    }

    return false;
}

static stock bool:VehicleIde_ParseLine(const line[]) {
    new field[GTA_VEHICLES_IDE_MAX_FIELD_LENGTH];

    if(!VehicleIde_GetCommaField(line, 0, field, sizeof(field)))
        return false;

    new modelId = strval(field);

    if(modelId <= 0)
        return false;

    new modelNameField[GTA_VEHICLES_IDE_MAX_FIELD_LENGTH];
    if(!VehicleIde_GetCommaField(line, 1, modelNameField, sizeof(modelNameField)))
        return false;

    new modelName[GTA_VEHICLES_IDE_NAME_LENGTH];
    if(!VehicleIde_GetFirstToken(modelNameField, modelName, sizeof(modelName)))
        return false;

    /*
    * 20 years later we are still dealing with Rockstar's typos, fix "dodo,emperor,wayfarer" whitespace
    */
    new bool:mergedModelTxd =
        VehicleIde_ContainsWhitespace(modelNameField);

    new base = mergedModelTxd ? 0 : 1;

    new txdName[GTA_VEHICLES_IDE_NAME_LENGTH];
    if(mergedModelTxd) {
        new pos = strlen(modelName);
        while(modelNameField[pos] == ' ' || modelNameField[pos] == '\t')
            pos++;

        if(modelNameField[pos] == '\0') {
            format(txdName, sizeof(txdName), "%s", modelName);
        } else {
            if(!VehicleIde_CopyTrimmedRange(modelNameField, pos, strlen(modelNameField), txdName, sizeof(txdName))) {
                return false;
            }
        }
    } else if(!VehicleIde_GetCommaField(line, 2, txdName, sizeof(txdName))) {
        return false;
    }

    new typeName[GTA_VEHICLES_IDE_TYPE_LENGTH];
    new handlingId[GTA_VEHICLES_IDE_HANDLING_ID_LENGTH];
    new gameName[GTA_VEHICLES_IDE_GAME_NAME_LENGTH];
    new anims[GTA_VEHICLES_IDE_ANIMS_LENGTH];
    new vehicleClass[32];

    if(!VehicleIde_GetCommaField(line, 2 + base, typeName, sizeof(typeName)))
        return false;
    if(!VehicleIde_GetCommaField(line, 3 + base, handlingId, sizeof(handlingId)))
        return false;
    if(!VehicleIde_GetCommaField(line, 4 + base, gameName, sizeof(gameName)))
        return false;
    if(!VehicleIde_GetCommaField(line, 5 + base, anims, sizeof(anims)))
        return false;
    if(!VehicleIde_GetCommaField(line, 6 + base, vehicleClass, sizeof(vehicleClass)))
        return false;

    if(!VehicleIde_GetCommaField(line, 7 + base, field, sizeof(field)))
        return false;
    new frequency = strval(field);

    if(!VehicleIde_GetCommaField(line, 8 + base, field, sizeof(field)))
        return false;
    new flags = strval(field);

    if(!VehicleIde_GetCommaField(line, 9 + base, field, sizeof(field)))
        return false;
    new componentRules = VehicleIde_ParseHex(field);

    if(gVehicleIdeCount >= GTA_VEHICLES_IDE_MAX_VEHICLES) {
        printf("[omp-gta-vehicles-ide] ERROR: Maximum vehicle count reached (%d).", GTA_VEHICLES_IDE_MAX_VEHICLES);
        return false;
    }

    new index = gVehicleIdeCount;

    gVehicleIde[index][GTA_VEHICLE_IDE_MODEL_ID] = modelId;
    gVehicleIde[index][GTA_VEHICLE_IDE_TYPE] = VehicleIde_GetType(typeName);
    gVehicleIde[index][GTA_VEHICLE_IDE_CLASS] = VehicleIde_GetClass(vehicleClass);
    gVehicleIde[index][GTA_VEHICLE_IDE_FREQUENCY] = frequency;
    gVehicleIde[index][GTA_VEHICLE_IDE_FLAGS] = flags;
    gVehicleIde[index][GTA_VEHICLE_IDE_COMPONENT_RULES] = componentRules;

    format(gVehicleIde[index][GTA_VEHICLE_IDE_NAME], GTA_VEHICLES_IDE_NAME_LENGTH, "%s", modelName);
    format(gVehicleIde[index][GTA_VEHICLE_IDE_TXD_NAME], GTA_VEHICLES_IDE_NAME_LENGTH, "%s", txdName);
    format(gVehicleIde[index][GTA_VEHICLE_IDE_TYPE_NAME], GTA_VEHICLES_IDE_TYPE_LENGTH, "%s", typeName);
    format(gVehicleIde[index][GTA_VEHICLE_IDE_HANDLING_ID], GTA_VEHICLES_IDE_HANDLING_ID_LENGTH, "%s", handlingId);
    format(gVehicleIde[index][GTA_VEHICLE_IDE_GAME_NAME], GTA_VEHICLES_IDE_GAME_NAME_LENGTH, "%s", gameName);
    format(gVehicleIde[index][GTA_VEHICLE_IDE_ANIMS], GTA_VEHICLES_IDE_ANIMS_LENGTH, "%s", anims);

    /*
    * In gta-reversed "extra" fields are not loaded, we read them anyway regardless if it is junk
    */
    gVehicleIde[index][GTA_VEHICLE_IDE_HAS_EXTRA_DATA] = false;
    gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_MODEL_ID] = -1;
    gVehicleIde[index][GTA_VEHICLE_IDE_FRONT_WHEEL_SCALE] = 0.0;
    gVehicleIde[index][GTA_VEHICLE_IDE_REAR_WHEEL_SCALE] = 0.0;
    gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_UPGRADE_CLASS] = -1;

    if(VehicleIde_GetCommaField(line, 10 + base, field, sizeof(field))) {
        gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_MODEL_ID] = strval(field);

        if(VehicleIde_GetCommaField(line, 11 + base, field, sizeof(field)))
            gVehicleIde[index][GTA_VEHICLE_IDE_FRONT_WHEEL_SCALE] = floatstr(field);

        if(VehicleIde_GetCommaField(line, 12 + base, field, sizeof(field)))
            gVehicleIde[index][GTA_VEHICLE_IDE_REAR_WHEEL_SCALE] = floatstr(field);

        if(VehicleIde_GetCommaField(line, 13 + base, field, sizeof(field)))
            gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_UPGRADE_CLASS] = strval(field);

        gVehicleIde[index][GTA_VEHICLE_IDE_HAS_EXTRA_DATA] = true;
    }

    if(gVehicleIde[index][GTA_VEHICLE_IDE_CLASS] == 0 && strcmp(vehicleClass, "ignore", true)) {
        printf("[omp-gta-vehicles-ide] WARNING: Unknown class '%s' for model %d.", vehicleClass, modelId);
    }

    if (gVehicleIde[index][GTA_VEHICLE_IDE_TYPE] == GTA_VEHICLE_IDE_TYPE_UNKNOWN) {
        printf("[omp-gta-vehicles-ide] WARNING: Unknown vehicle type '%s' for model %d.", typeName, modelId);
    }

    gVehicleIdeCount++;
    return true;
}

/*
* --------------------------------------------------------------------------
* USABLE FUNCTIONS
* --------------------------------------------------------------------------
*/

stock bool:LoadVehiclesIde(const filename[] = GTA_VEHICLES_IDE_FILE) {
    new File:file = fopen(filename, io_read);

    if(!file){
        printf("[omp-gta-vehicles-ide] ERROR: Could not open '%s'.", filename);
        return false;
    }

    gVehicleIdeCount = 0;
    gVehicleIdeLoaded = false;

    new line[GTA_VEHICLES_IDE_MAX_LINE_LENGTH];
    new bool:inCarsSection = false;

    while(fread(file, line)) {
        new length = strlen(line);

        while(length > 0 && (line[length - 1] == '\r' || line[length - 1] == '\n')) {
            line[--length] = '\0';
        }

        if(VehicleIde_IsCommentOrEmpty(line))
            continue;

        if(!strcmp(line, "cars", true)) {
            inCarsSection = true;
            continue;
        }

        if(!strcmp(line, "end", true)) {
            if(inCarsSection)
                break;

            continue;
        }

        if(!inCarsSection)
            continue;

        if(!VehicleIde_ParseLine(line)) {
            printf("[omp-gta-vehicles-ide] WARNING: Failed to parse line: %s", line);
        }
    }

    fclose(file);

    if(gVehicleIdeCount <= 0) {
        printf("[omp-gta-vehicles-ide] ERROR: No vehicle entries loaded from '%s'.", filename);

        return false;
    }

    gVehicleIdeLoaded = true;

    printf("[omp-gta-vehicles-ide] Loaded %d vehicle entries from '%s'.", gVehicleIdeCount, filename);

    return true;
}

stock bool:IsVehicleIdeLoaded() {
    return gVehicleIdeLoaded;
}

/*
* Gets the vehicle model id from its vehicles.ide model name
*/
stock GetVehicleIdeModelIdByName(const name[]) {
    if(!gVehicleIdeLoaded)
        return -1;

    for(new i = 0; i < gVehicleIdeCount; i++) {
        if(!strcmp(gVehicleIde[i][GTA_VEHICLE_IDE_NAME], name, true)) {
            return gVehicleIde[i][GTA_VEHICLE_IDE_MODEL_ID];
        }
    }

    return -1;
}

/*
* Gets the vehicle's class (not type), such as normal, executive, richfamily
*/
stock GetVehicleIdeClassByModel(modelId) {
    if(!gVehicleIdeLoaded)
        return 0;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return 0;

    return gVehicleIde[index][GTA_VEHICLE_IDE_CLASS];
}

/*
* Gets the internal "model_name", which is useful to match the vehicle model in other game files.
* For example, Infernus, infernus, INFERNUS, INFERNU... this function returns "Infernus"
*/
stock bool:GetVehicleIdeNameByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;

    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_NAME]);
    return true;
}

/*
* Gets the vehicle's model type converted to a enum
*/
stock E_GTA_VEHICLE_IDE_TYPE:GetVehicleIdeTypeByModel(modelId) {
    if(!gVehicleIdeLoaded)
        return GTA_VEHICLE_IDE_TYPE_UNKNOWN;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return GTA_VEHICLE_IDE_TYPE_UNKNOWN;

    return gVehicleIde[index][GTA_VEHICLE_IDE_TYPE];
}

/*
* Gets the spawning frequency of a certain vehicle model. This value is used in singleplayer to determine partially which car to spawn, but is only a small part of a much larger logic
*/
stock GetVehicleIdeFrequencyByModel(modelId) {
    if(!gVehicleIdeLoaded)
        return 0;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return 0;
    return gVehicleIde[index][GTA_VEHICLE_IDE_FREQUENCY];
}

/*
* Gets the flags added to the vehicle model, not very useful since this field contains streaming related data
*/
stock GetVehicleIdeFlagsByModel(modelId) {
    if(!gVehicleIdeLoaded)
        return 0;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return 0;
    return gVehicleIde[index][GTA_VEHICLE_IDE_FLAGS];
}

/*
* Gets what kind of "components" can a car have on spawn, these are not Transfender modifications, but refers to an index inside .DFF files for vehicle variations such as ads on Coach
*/
stock GetVehicleIdeComponentRulesByModel(modelId) {
    if(!gVehicleIdeLoaded)
        return 0;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return 0;
    return gVehicleIde[index][GTA_VEHICLE_IDE_COMPONENT_RULES];
}

/*
* Gets the internal "TXD", which is useful to match the vehicle model in other game files.
* For example, Infernus, infernus, INFERNUS, INFERNU... this function returns "infernus"
*/
stock bool:GetVehicleIdeTxdNameByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;

    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_TXD_NAME]);
    return true;
}

/*
* Gets the vehicle's type name such as "car", "bike", "boat"
*/
stock bool:GetVehicleIdeTypeNameByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;
    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_TYPE_NAME]);
    return true;
}

/*
* Gets the internal "handling ID", which is useful to match the vehicle model in other game files.
* For example, Infernus, infernus, INFERNUS, INFERNU... this function returns "INFERNUS"
*/
stock bool:GetVehicleIdeHandlingIdByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;
    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_HANDLING_ID]);
    return true;
}

/*
* Gets the internal "game_name", which is useful to match the vehicle model in other game files.
* For example, Infernus, infernus, INFERNUS, INFERNU... this function returns "INFERNU"
*/
stock bool:GetVehicleIdeGameNameByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;
    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_GAME_NAME]);
    return true;
}

/*
* Gets the animation group related to the vehicle model
*/
stock bool:GetVehicleIdeAnimsByModel(modelId, output[], size = sizeof(output)) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1)
        return false;
    format(output, size, "%s", gVehicleIde[index][GTA_VEHICLE_IDE_ANIMS]);
    return true;
}

/*
* Gets the vehicle model's wheel details such as the default spawning wheel model, how large are the wheels and what kind of wheels can be installed
*/
stock bool:GetVehicleIdeWheelDataByModel(modelId, &wheelModelId, &Float:frontWheelScale, &Float:rearWheelScale, &wheelUpgradeClass) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);

    if(index == -1 || !gVehicleIde[index][GTA_VEHICLE_IDE_HAS_EXTRA_DATA])
        return false;

    wheelModelId = gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_MODEL_ID];
    frontWheelScale = gVehicleIde[index][GTA_VEHICLE_IDE_FRONT_WHEEL_SCALE];
    rearWheelScale = gVehicleIde[index][GTA_VEHICLE_IDE_REAR_WHEEL_SCALE];
    wheelUpgradeClass = gVehicleIde[index][GTA_VEHICLE_IDE_WHEEL_UPGRADE_CLASS];

    return true;
}

/*
* Checks if a modelid belongs to a certain vehicle group
*/
stock bool:IsVehicleIdeType(modelId, E_GTA_VEHICLE_IDE_TYPE:type) {
    if(!gVehicleIdeLoaded)
        return false;

    new index = VehicleIde_GetIndex(modelId);
    if (index == -1)
        return false;

    return gVehicleIde[index][GTA_VEHICLE_IDE_TYPE] == type;
}

/*
* Checks if modelid is found in vehicles.ide file
*/
stock bool:IsVehicleIdeModel(modelId) {
    if(!gVehicleIdeLoaded)
        return false;

    return VehicleIde_GetIndex(modelId) != -1;
}
