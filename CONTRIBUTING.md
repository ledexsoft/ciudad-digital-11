# Cómo contribuir

Gracias por querer ayudar con Ciudad Digital 1.1. La meta inicial es explicar el mapa y permitir recorrer a pie una zona documentada de Cumanayagua; primero necesitamos que el terreno, la posición del jugador y la experiencia básica sean verificables.

## Antes de empezar

1. Lee el [estado y la guía para desarrolladores](https://ledexsoft.com/Proyectos/CiudadDigital11/) y revisa las tareas abiertas en [GitHub Issues](https://github.com/ledexsoft/ciudad-digital-11/issues).
2. Para trabajo en Unreal se necesita Unreal Engine **5.8** y una copia propia del proyecto y de sus assets autorizados. Este repositorio es solo código fuente: no distribuye el `.uproject`, mapas, Blueprints ni assets de plantilla.
3. Comenta en una incidencia qué problema vas a resolver antes de empezar un cambio grande. Para errores pequeños de código o documentación, puedes abrir directamente un pull request.

## Flujo de trabajo

1. Haz un fork del repositorio y crea una rama enfocada, por ejemplo `docs/estado-terreno` o `fix/solar-timezone`.
2. Mantén cada pull request centrado en un objetivo y explica el problema, el cambio y cómo lo verificaste.
3. Si modificas C++, compila `CumaPlayEditor` para UE 5.8. Si cambia comportamiento de juego o assets, indica además los pasos usados para probarlo en PIE o standalone; una compilación correcta no sustituye una prueba en el editor.
4. No afirmes que una característica funciona visualmente hasta ejecutarla. Si no puedes probarla, deja esa limitación clara.

## Datos geográficos y assets

- Para cualquier dato geográfico registra proveedor, fecha, área, resolución, CRS/datum, licencia y atribución. No inventes alturas, fachadas o detalles locales.
- No subas tiles, capturas, modelos ni archivos de contenido de Unreal/Marketplace/Epic salvo que la licencia permita expresamente redistribuirlos en formato fuente. El `.gitignore` excluye `Content/`, `SourceData/`, `Config/`, `.codex/`, cachés y salidas generadas.
- No incluyas credenciales, tokens, archivos de configuración local ni cachés de Unreal.

## Pull requests

Incluye un resumen corto, los archivos o sistemas afectados, el resultado de compilación/verificación y cualquier limitación pendiente. Mantén la atribución y licencia de cada recurso externo y no cambies la licencia MIT del código del proyecto para cubrir contenido de terceros.
