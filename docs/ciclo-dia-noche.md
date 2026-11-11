# Ciclo solar ligado al reloj del dispositivo

## Implementación

- `Source/CumaPlay/` añade el módulo C++ runtime del proyecto Blueprint.
- `UCumaDayNightSubsystem` es un `UTickableWorldSubsystem`: se crea en mundos de juego/PIE y actualiza la iluminación una vez por segundo. No requiere un actor o Blueprint colocado a mano.
- Se habilita el plugin runtime oficial `SunPosition` de UE 5.8. Su función solar calcula azimut, elevación corregida por refracción, amanecer y atardecer.
- La ubicación usada es la del proyecto: 22.14938° N, 80.20212° O, Cumanayagua.
- La posición solar se calcula con `FDateTime::UtcNow()`, que representa el instante real sin ambigüedad por zona horaria. En paralelo, `FDateTime::Now()` y la diferencia `Now - UtcNow` determinan la zona horaria y el horario de verano configurados por el sistema. Se usan para devolver la hora civil local y convertir amanecer/atardecer a la hora local del dispositivo. Así, una computadora en otra zona horaria sigue mostrando el sol en la posición correcta sobre Cumanayagua.
- El horario de verano ya viene incorporado en la diferencia entre hora local y UTC; se pasa UTC y offset cero al cálculo solar para no aplicar el horario de verano dos veces.
- Se orienta la luz direccional siguiendo el azimut/elevación solares. La orientación convierte el norte geográfico del proyecto (−Y; +X es este) a la rotación de Unreal.
- Una luz direccional existente se selecciona por etiqueta `CumaSun`; si no existe etiqueta, se prefiere la luz marcada como sol de Sky Atmosphere y luego la primera direccional. Si el nivel no contiene una luz direccional, el sistema crea una al iniciar el juego.
- Si faltan, se crean también `SkyAtmosphere` y `SkyLight`. En Windows/macOS la Skylight usa captura en tiempo real para que el color ambiental y las reflexiones sigan al cielo. En Android/iOS se omite esa captura continua y se aproxima el ambiente mediante intensidad y tinte, para reducir el coste.
- La luz directa se atenúa al pasar por el crepúsculo y cambia su temperatura de color cerca del horizonte. El sistema expone en Blueprint hora local, elevación/azimut y horas locales de salida y puesta del sol.

## Alcance y limitaciones

Esto implementa una **posición solar astronómica real**, sincronizada al reloj y zona horaria del sistema operativo. La curva de intensidad ambiental es una aproximación artística; no es una simulación radiométrica completa. No incluye luna, fase lunar, clima, nubes dinámicas propias ni alumbrado urbano. Las luces artificiales se podrán sumar después con el sistema de ciudad.

El cielo y la Skylight se crean en runtime si hacen falta; no se altera el nivel de terreno ni se requieren cambios en los assets de luz. El sistema se ejecuta en cualquier mapa de juego que cargue el módulo y aplica la iluminación al mundo activo.

## APIs oficiales consultadas

- [Sun Position Calculator plugin (UE 5.8)](https://dev.epicgames.com/documentation/unreal-engine/API/PluginIndex/SunPosition) y [herramienta geográfica Sun Position (UE 5.8)](https://dev.epicgames.com/documentation/unreal-engine/geographically-accurate-sun-positioning-tool-in-unreal-engine) — plugin incluido en la instalación; su módulo runtime aporta `USunPositionFunctionLibrary::GetSunPosition`.
- [FDateTime (UE 5.8)](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/FDateTime) — `Now()` entrega hora local y `UtcNow()` entrega UTC.
- [Actor Sun and Sky (UE 5.8)](https://dev.epicgames.com/documentation/en-us/unreal-engine/sun-and-sky-actor-in-unreal-engine) y [Sky Light real-time capture (UE 5.8)](https://dev.epicgames.com/documentation/unreal-engine/sky-lights-in-unreal-engine) — referencia oficial para sol geográfico y captura dinámica compatible con hora del día.
- [Directional Lights (UE 5.8)](https://dev.epicgames.com/documentation/unreal-engine/directional-lights-in-unreal-engine) — movilidad y luz solar dinámica.

## Verificación

El módulo compiló correctamente para `CumaPlayEditor Mac Development` con la instalación de Unreal Engine 5.8, sin abrir el editor. La comprobación visual en PIE/standalone sigue pendiente para confirmar orientación de amanecer/atardecer, exposición y coste de captura en la M1. No se han ejecutado pruebas automatizadas.
