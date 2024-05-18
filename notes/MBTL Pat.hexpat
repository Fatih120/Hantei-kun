#pragma pattern_limit 1800000
#include <type/magic.pat>
enum Flip : char {
  None,    // 0x00
  H,
  V,
  HV
};
enum Trans : char {
  None,    // 0x00
  Add,
  Sub
};
enum ShapeType : char {
  None,
  Plane,
  Unknown,
  Ring,
  Pie,
  Sphere,
  Cone
};
struct PRTag {
    char pTag[4] [[hidden]];
    match(pTag){
        ("PRXY") : {str name="xy";s32 xy[2];}
        ("PRAL") : {str name = "trans";Trans trans;}
        ("PRRV") : {str name = "flip";Flip flip;}
        ("PRFL") : {str name = "flter";bool filter;}
        ("PRZM") : {str name = "scale";float scale[2];}
        ("PRSP") : {str name = "addColor";u8 addColor[4];}
        ("PRAN") : {str name = "PRAN";u32 PRAN;}
        ("PRPR") : {str name = "priority";u32 priority;}
        ("PRID") : {str name = "prId";u32 prId;}
        ("PRCL") : {str name = "color";u8 color[4];}
        ("PRA3") : {str name = "rotation";padding[4];float rotation[3];}
        (_) : break;
    }
} [[sealed,name(std::format("{}", name)),format("fmt_PR")]];
fn fmt_PR(PRTag data){
    match (data.pTag) {
        ("PRXY") : {return std::format("{}", data.xy);}
        ("PRAL") : {return std::format("{}", data.trans) ;}
        ("PRRV") : {return std::format("{}", data.flip) ;}
        ("PRFL") : {return std::format("{}", data.filter) ;}
        ("PRZM") : {return std::format("{}", data.scale) ;}
        ("PRSP") : {return std::format("{}", data.addColor) ;}
        ("PRAN") : {return std::format("{}", data.PRAN) ;}
        ("PRPR") : {return std::format("{}", data.priority) ;}
        ("PRID") : {return std::format("{}", data.prId) ;}
        ("PRCL") : {return std::format("{}", data.color) ;}
        ("PRA3") : {return std::format("{}", data.rotation) ;}
        (_) : {return "unknown";} 
    }
};

struct PRST {
    padding[4];
    u32 id [[hidden]];
    PRTag tags[while(builtin::std::mem::read_string($, 4) != "PRED")] [[inline]];
    //char pred[4];
    type::Magic<"PRED"> end [[hidden]];
} [[name(id)]];
struct Pattern {
    padding[4];
    u32 id [[hidden]];
    if(builtin::std::mem::read_string($, 4) == "PANA"){
        padding[4];
        char length [[hidden]];
        char name[length] [[hidden]];
    }
    if(builtin::std::mem::read_string($, 4) == "PRST"){
        PRST prst[while(builtin::std::mem::read_string($, 4) == "PRST")] [[inline]];
    }
    type::Magic<"P_ED"> end [[hidden]];
} [[name(std::format("{}({})",id,name))]];

struct PPTag {
    char pTag[4] [[hidden]];
    match(pTag){
        ("PPCC") : {str name = "xy";s32 data[2];}
        ("PPUV") : {str name = "uv";s32 data[4];}
        ("PPSS") : {str name = "wh";s32 data[2];}
        ("PPTE") : {str name = "unknown";u16 data[2];}
        ("PPPA") : {str name = "colorSlot";u32 data;}
        ("PPTP") : {str name = "textureId";u32 data;}
        ("PPPP") : {str name = "shapeIndex";u32 data;}
        ("PPTX") : {str name = "unknown";u32 data;}
        ("PPJP") : {str name = "unknwn xy offset";s32 data[2];}
        (_) : break;
    }
} [[sealed,name(std::format("{}", name)),format("fmt_PP")]];
fn fmt_PP(PPTag data){
    return std::format("{}", data.data);
};
struct PPST {
    padding[4];
    u32 id [[hidden]];
    if(builtin::std::mem::read_string($, 4) == "PPNA"||builtin::std::mem::read_string($, 4) == "PPNM"){
        padding[4];
        char length [[hidden]];
        char name[length] [[hidden]];
    }
    PPTag tags[while(builtin::std::mem::read_string($, 4) != "PPED")] [[inline]];
    //padding[4];
    type::Magic<"PPED"> end [[hidden]];
} [[name(std::format("{}({})", id, name))]];


struct VEST {
    ShapeType type;
    padding[3];
    u32 params[15];
};
struct TexData {
    s32 size;
    s32 w;
    s32 h;
    u32 type;
    if(type == 5 || size == w*h+128){
        padding[8+w*h+32*4];
    } else {
        padding[16];
        u32 cSize;
        u32 oSize;
        padding[cSize];
    }
};
struct PGTag {
    char pTag[4] [[hidden]];
    match(pTag){
        ("PGTE") : {str name = "size";s16 data[2];}
        ("PGT2") : {str name = "data";TexData data;}
        (_) : break;
    }
} [[name(std::format("{}", name)),format("fmt_PG")]];
fn fmt_PG(PGTag data){
    return std::format("{}", data.data);
};

struct PGST {
    padding[4];
    u32 id [[hidden]];
    if(builtin::std::mem::read_string($, 4) == "PGNM"){
        padding[4];
        char name[32] [[hidden]];
    }
    PGTag tags[while(builtin::std::mem::read_string($, 4) != "PGED")] [[inline]];
    type::Magic<"PGED"> end [[hidden]];
} [[name(std::format("{}({})", id, name))]];

struct Part {
    Pattern patterns[while(builtin::std::mem::read_string($, 4) == "P_ST")];
    PPST cutbox[while(builtin::std::mem::read_string($, 4) == "PPST")];
    if(builtin::std::mem::read_string($, 4) == "VEST"){    
        padding[4];
        u32 length [[hidden]];
        u32 size [[hidden]];
        VEST shapes[length];
        type::Magic<"VNST"> vnst [[hidden]];
        padding[4*8*length];
        type::Magic<"VEED"> veed [[hidden]];
    }
    PGST textures[while(builtin::std::mem::read_string($, 4) == "PGST")];
};
Part p @ 0x24;