# Documento tecnico: entradas, magnitudes y algoritmos de la simulacion

## 1) Entradas que usa la simulacion y su magnitud

Este documento describe la version activa del notebook `Robot_Futbol_Lobo_Gris (1).ipynb` (celda principal con `SimuladorFutbol`).

### 1.1 Entorno fisico (cancha)

| Variable | Valor | Unidad | Uso |
|---|---:|---|---|
| `ancho` | 150 | cm | Limites en eje X |
| `largo` | 300 | cm | Limites en eje Y |
| `porteria_a` | (75, 10) | cm | Referencia de gol A |
| `porteria_b` | (75, 290) | cm | Objetivo de ataque |
| `zona_inicio_a` | (75, 80) | cm | Inicio Robot A |
| `zona_inicio_b` | (75, 220) | cm | Inicio Robot B |
| Obstaculos | (45,145,r12), (105,175,r12), (75,220,r10) | cm | Evitacion y mapeo |

### 1.2 Estado del robot

| Entrada/estado | Rango o tipo | Unidad | Como se usa |
|---|---|---|---|
| `x, y` | [5, ancho-5], [5, largo-5] | cm | Posicion actual |
| `rol` | `BUSCADOR`, `ATACANTE`, `ASISTENTE` | categoria | Control de comportamiento |
| `tiene_balon` | `True/False` | booleano | Condicion para atacar/gol |
| `radio_robot` | 5 | cm | Colision con obstaculos |

### 1.3 Sensor Sharp (distancia)

Parametros:
- `rango_min = 4 cm`
- `rango_max = 80 cm`
- ruido de voltaje: `N(0, 0.05)`

Modelo usado:
```text
si distancia_real > 80 -> no lectura (None)
distancia_util = max(4, distancia_real)
voltaje = 3.0 - (distancia_util/80)*2.7 + ruido
voltaje acotado a [0.3, 3.0]
distancia_estimada = (3.0 - voltaje)*80/2.7
distancia_estimada acotada a [4, 80]
```

Uso principal:
- Triangulacion (`r1`, `r2`)
- Posesion (con umbral de posesion)

### 1.4 Sensor color (RGB sintetico)

Senales sinteticas en cada paso (segun distancia al balon):
```text
proximidad = max(0, 1 - distancia_real/120)
frec_r = max(1, 120 + 90*proximidad + N(0,6))
frec_g = max(1,  85 + 35*proximidad + N(0,6))
frec_b = max(1, 110 - 55*proximidad + N(0,6))
```

Score naranja:
```text
total = r + g + b
r_norm = r/total
g_norm = g/total
b_norm = b/total
score_naranja = 0.8*r_norm + 0.4*g_norm - 0.3*b_norm
```

Deteccion:
```text
color_detectado = score_naranja > umbral
umbral inicial = 0.50
```

Auto-calibracion (si hay evidencia fuerte):
- condicion: `sharp_distancia <= 25` y `color_detectado == True`
- actualizacion:
```text
objetivo = clip(score - 0.02, 0.45, 0.65)
umbral = (1-alpha)*umbral + alpha*objetivo
alpha = 0.08
```

### 1.5 Encoder

| Parametro | Valor |
|---|---:|
| `pulsos_por_revolucion` | 12 |
| radio rueda (implicito) | 2.3 cm |
| circunferencia | `2*pi*2.3` cm |

Relacion:
```text
rpm = (velocidad_cm_s * 60) / circunferencia_rueda
```

### 1.6 Umbrales de control

| Parametro | Valor actual | Uso |
|---|---:|---|
| `dt` | 0.5 s | Paso de simulacion |
| `umbral_posesion` | 20 cm | Requiere `sharp<=umbral` y color detectado |
| `umbral_gol` | 8 cm | Distancia a `porteria_b` con balon |
| Ventana mapa global | 12 pasos | Detecciones recientes |
| Ventana estimacion reciente | 35 pasos | Habilita persecucion por estimado |

---

## 2) Dump de datos RGB para regresion multivariable

Se genero un dump de ejemplo con 30 filas en:

`C:\Users\wsteb\Downloads\dump_rgb_regresion_30_muestras.csv`

Columnas:
- `frec_r, frec_g, frec_b` (features RGB)
- `distancia_real_cm` (feature adicional)
- `balon_presente` (feature binaria de contexto)
- `score_naranja_objetivo` (target continuo `y`)
- `etiqueta_naranja` (target binario derivado)

Ejemplo (primeras 10 filas):

| frec_r | frec_g | frec_b | distancia_real_cm | balon_presente | score_naranja_objetivo (y) | etiqueta_naranja |
|---:|---:|---:|---:|---:|---:|---:|
| 111.8542 | 96.1710 | 103.5364 | 74.4752 | 0 | 0.2985 | 0 |
| 203.1398 | 115.1852 | 73.8329 | 23.7808 | 1 | 0.4680 | 0 |
| 209.5199 | 130.9264 | 69.5848 | 11.5460 | 1 | 0.5064 | 1 |
| 128.5228 | 91.8045 | 120.2469 | 114.0707 | 1 | 0.3097 | 0 |
| 113.0853 | 87.5207 | 128.2520 | 116.0133 | 0 | 0.2521 | 0 |
| 129.5074 | 93.2416 | 117.0792 | 97.7741 | 1 | 0.3273 | 0 |
| 115.7776 | 69.1331 | 130.0032 | 39.3352 | 0 | 0.2584 | 0 |
| 208.3686 | 113.3398 | 61.6449 | 15.3300 | 1 | 0.5054 | 1 |
| 101.4116 | 94.6396 | 99.6845 | 83.3710 | 0 | 0.2832 | 0 |
| 191.9690 | 112.3206 | 89.3068 | 55.0577 | 1 | 0.4214 | 0 |

### 2.1 Como se calcula la variable objetivo y (regresion)

En el dataset sintetico, `y = score_naranja_objetivo` se construye asi:

```text
r_norm = frec_r / (frec_r + frec_g + frec_b)
g_norm = frec_g / (frec_r + frec_g + frec_b)
b_norm = frec_b / (frec_r + frec_g + frec_b)

y = 0.8*r_norm + 0.4*g_norm - 0.3*b_norm + N(0, 0.015)
y = clip(y, 0, 1)
```

Y la etiqueta binaria asociada es:
```text
etiqueta_naranja = 1 si y > 0.50, en otro caso 0
```

Nota: para entrenar regresion multivariable, se pueden usar como `X`:
`[frec_r, frec_g, frec_b, distancia_real_cm, balon_presente]`
y como `y`:
`score_naranja_objetivo`.

---

## 3) Triangulacion de posicion del balon (conceptual)

### 3.1 Entradas necesarias

1. Posicion robot A: `(x1, y1)`
2. Posicion robot B: `(x2, y2)`
3. Distancia estimada Sharp A: `r1`
4. Distancia estimada Sharp B: `r2`
5. Ultimo estimado de balon `(balon_est_x, balon_est_y)` para continuidad temporal

### 3.2 Logica usada

Caso A: hay `r1` y `r2` validos  
- Se intersectan dos circulos:
  - circulo A: centro `(x1,y1)`, radio `r1`
  - circulo B: centro `(x2,y2)`, radio `r2`
- Puede haber 2 puntos candidatos (`c1`, `c2`).
- Se elige el candidato mas cercano al estimado previo (suaviza saltos).

Caso B: solo hay una lectura valida (`r1` o `r2`)  
- Se proyecta desde ese robot en la direccion del estimado previo, con modulo `r`.

Caso C: no hay lecturas validas  
- Se mantiene el estimado previo hasta nueva evidencia.

Acotacion:
- Todo estimado se fuerza a limites de cancha (`x,y` en margenes seguros).

---

## 4) Exploracion del entorno y adaptacion del robot

### 4.1 Flujo por paso

1. Actualiza sensores (Sharp + color + encoder)
2. Triangula balon estimado
3. Actualiza percepcion y mapa local/global
4. Evalua posesion
5. Asigna rol (GWO simplificado)
6. Ejecuta movimiento
7. Actualiza balon real y chequea gol

### 4.2 Exploracion

Cuando no hay evidencia suficiente para persecucion directa, el robot hace barrido orbital:
```text
angulo_exploracion += 0.35
radio = 45 (Robot A) o 55 (Robot B)
tx = cx + radio*cos(angulo)
ty = cy + radio*sin(0.9*angulo)
```
Esto produce cobertura espacial alrededor de zonas centrales desplazadas.

### 4.3 Adaptacion y memoria

- **Mapa local por robot**: guarda detecciones de `obstaculo` y `balon_candidato` con `(x,y,confianza,paso)`.
- **Mapa global reciente**: fusion de ambos robots con ventana temporal (12 pasos).
- **Persecucion por estimacion reciente**: aunque no haya deteccion directa instantanea, si hubo estimacion util reciente (<=35 pasos), se mantiene persecucion hacia `(balon_est_x, balon_est_y)`.
- **Auto-calibracion de color**: ajusta dinamicamente el umbral para compensar variaciones de lectura.

---

## 5) Archivos relevantes

- Notebook principal:
  - `C:\Users\wsteb\Downloads\Robot_Futbol_Lobo_Gris (1).ipynb`
- Dataset completo para regresion:
  - `C:\Users\wsteb\Downloads\datos_regresion_sensor_color.csv`
- Dump de ejemplo RGB/regresion (30 filas):
  - `C:\Users\wsteb\Downloads\dump_rgb_regresion_30_muestras.csv`
