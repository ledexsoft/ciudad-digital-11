# Ciudad Digital 1.1 — Cumanayagua

Código C++ y documentación para una simulación explorable de Cumanayagua, Cienfuegos, creada para Unreal Engine 5.8.

## Qué incluye este repositorio

- El módulo runtime `CumaPlay`, incluido el ciclo solar basado en la hora real del dispositivo y la ubicación del proyecto.
- Ajustes runtime de colisión y apoyo en el terreno para el personaje de primera persona de la plantilla de Unreal.
- Documentación de implementación y verificación.

Documentación del proyecto: [plan, estado y guía para desarrolladores](https://ledexsoft.com/Proyectos/CiudadDigital11/). Para proponer o enviar cambios, consulta [CONTRIBUTING.md](CONTRIBUTING.md).

Este es un repositorio de **código fuente**, no el proyecto Unreal completo. No contiene mapas ni assets `.uasset`/`.umap`, contenido de plantillas de Epic, datos geográficos externos, configuración local, cachés ni binarios. Para usar el código, integra `Source/CumaPlay` en un proyecto propio de UE 5.8, habilita el plugin incluido `SunPosition` y añade los mapas/assets que tengas derecho a usar. El código aquí no hace que esos recursos estén cubiertos por la licencia MIT.

## Estado

- El módulo compiló para `CumaPlayEditor Mac Development` con Unreal Engine 5.8 en Apple Silicon.
- La ejecución en PIE y la validación visual de las funciones siguen pendientes.
- Windows y Android/iOS no se han compilado ni verificado.

## Licencia

El código y la documentación originales de este repositorio están bajo la licencia MIT, en [LICENSE](LICENSE). Unreal Engine, las plantillas de Epic y los datos geográficos o imágenes de terceros conservan sus propias licencias; esos archivos se excluyen de este repositorio.
