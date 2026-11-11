# Colisión de primera persona con el terreno

El mapa conserva un `PlayerStart` y el Landscape tiene componentes de colisión vinculados a sus datos de altura. La colisión de un personaje `ACharacter` no la hacen los pies visibles: Unreal usa la cápsula del personaje para barridos, apoyo en el suelo, escalones y pendientes.

`UCumaCharacterCollisionSubsystem` espera a que aparezca el personaje de la plantilla y aplica una sola vez estos ajustes:

- Activa colisión de consulta/física en la cápsula y fuerza respuesta de bloqueo al canal `WorldStatic`, que incluye el Landscape.
- Mantiene la forma y tamaño de cápsula definidos por el Blueprint de la plantilla.
- Activa comprobación del suelo con base plana para reducir el deslizamiento y la pérdida gradual de apoyo cerca de bordes.
- Usa un ángulo máximo caminable de 50°, altura de escalón de 45 cm y umbral de percha de 10 cm; elimina la altura adicional de percha para que no quede suspendido sobre una arista pequeña.
- Sigue permitiendo caminar fuera de un borde real.

El subsistema busca al jugador cada 0,1 segundos solo mientras no lo encuentra y deja de actualizarse después de aplicar la configuración. La implementación está en `Source/CumaPlay/Private/CumaCharacterCollisionSubsystem.cpp`.

La compilación del target `CumaPlayEditor Mac Development` en UE 5.8 terminó correctamente. No se abrió Unreal ni se ejecutó PIE; por eso siguen pendientes la verificación de que los pies se vean apoyados, el ascenso/descenso de pendientes reales y la inspección de la respuesta del Landscape al caminar.
