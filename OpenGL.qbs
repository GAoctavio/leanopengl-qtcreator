import qbs

CppApplication {
    type: "application" // To suppress bundle generation on Mac
    consoleApplication: true
    files: [
        "Box/Cardboard.mtl",
        "Box/Cardboard.obj",
        "Box/black.jpg",
        "Box/cardboard.png",
        "Box/cardboard1.jpg",
        "Box/green.png",
        "Box/tore_card.png",
        "Box/tore_cardboard.jpg",
        "Box/white.jpg",
        "CartoonGuy/CartoonGuy_v1.1.mtl",
        "FuturisticCar/Futuristic_Car.mtl",
        "FuturisticCar/Futuristic_Car.obj",
        "Lightbulb/energy_saver-cycles.mtl",
        "Lightbulb/energy_saver-cycles.obj",
        "Lightbulb/energy_saving_lightbulb.mtl",
        "Lightbulb/energy_saving_lightbulb.obj",
        "Nanosuit/arm_dif.png",
        "Nanosuit/arm_showroom_ddn.png",
        "Nanosuit/arm_showroom_spec.png",
        "Nanosuit/body_dif.png",
        "Nanosuit/body_showroom_ddn.png",
        "Nanosuit/body_showroom_spec.png",
        "Nanosuit/glass_ddn.png",
        "Nanosuit/glass_dif.png",
        "Nanosuit/hand_dif.png",
        "Nanosuit/hand_showroom_ddn.png",
        "Nanosuit/hand_showroom_spec.png",
        "Nanosuit/helmet_diff.png",
        "Nanosuit/helmet_showroom_ddn.png",
        "Nanosuit/helmet_showroom_spec.png",
        "Nanosuit/leg_dif.png",
        "Nanosuit/leg_showroom_ddn.png",
        "Nanosuit/leg_showroom_spec.png",
        "Nanosuit/nanosuit.blend",
        "Nanosuit/nanosuit.mtl",
        "Nanosuit/nanosuit.obj",
        "awesomeface.png",
        "camera.h",
        "container.jpg",
        "container2.png",
        "container2_emission.jpg",
        "container2_specular.png",
        "fragment_shader.frag",
        "lamp_shader.frag",
        "lamp_shader.vert",
        "lighting_shader.frag",
        "lighting_shader.vert",
        "main.cpp",
        "mesh.h",
        "model.h",
        "shader.cpp",
        "shader.h",
        "vertex_shader.vert",
    ]
    cpp.cppFlags: ["-std=c++14","-g"]
    cpp.dynamicLibraries:["assimp","SOIL","GLEW","glfw","GL","X11","pthread","Xrandr","Xi"]
    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
}

