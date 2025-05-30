#!/usr/bin/env python3
"""
The Computer Language Benchmarks Game
https://salsa.debian.org/benchmarksgame-team/benchmarksgame/

Contributed by Sokolov Yura, Isaac Gouy
Modified by Tupteq, Fredrik Johansson
"""
import pyperf
import math
import sys # Added to access float methods if they were not built-in
# import functools # No longer needed for this approach

# N-body for the Computer Language Benchmarks Game
#
# http://benchmarksgame.alioth.debian.org/
#
# Contributed by Sokolov Yura
# Modified by Isaac Gouy
#
# To run:
#
# python nbody.py 50000000

# Slightly modified by Tupteq to run on a certain old version of Python
#
# Further modified by Fredrik Johansson.

DEFAULT_ITERATIONS = 20000
# DEFAULT_REFERENCE = 'solarsystem' # Hardcoding for now


BODIES = {
    'sun': ([0.0, 0.0, 0.0], [0.0, 0.0, 0.0], 1.0),

    'jupiter': ([4.84143144246472090e+00,
                 -1.16032004402742839e+00,
                 -1.03622044471123109e-01],
                [1.66007664274403694e-03 * 365.25,
                 7.69902744105501363e-03 * 365.25,
                 -6.90460016972063023e-05 * 365.25],
                9.54791938424326609e-04),

    'saturn': ([8.34336671824457987e+00,
                4.12479856412430479e+00,
                -4.03523417114321381e-01],
               [-2.76742510726862411e-03 * 365.25,
                4.99852801234917238e-03 * 365.25,
                2.30417297573763929e-05 * 365.25],
               2.85885980666130812e-04),

    'uranus': ([1.28943695621391310e+01,
                -1.51111514016986312e+01,
                -2.23307578892655734e-01],
               [2.96460137564761618e-03 * 365.25,
                2.37847173959480950e-03 * 365.25,
                -2.96589568540237556e-05 * 365.25],
               4.36624404335156298e-05),

    'neptune': ([1.53796971148509165e+01,
                 -2.59193146099879641e+01,
                 1.79258772950371181e-01],
                [2.68067777249038932e-03 * 365.25,
                 1.62824170038242295e-03 * 365.25,
                 -9.51592254519715870e-05 * 365.25],
                5.15138902046611451e-05)
}

SYSTEM = list(BODIES.values())
PAIRS = []
for i in range(len(SYSTEM)):
    for j in range(i + 1, len(SYSTEM)):
        PAIRS.append((SYSTEM[i], SYSTEM[j]))


def advance(dt, n, bodies, pairs):
    for i in range(n):
        for (body1, body2) in pairs:
            pos1, vel1, mass1 = body1
            pos2, vel2, mass2 = body2

            dx = pos1[0] - pos2[0]
            dy = pos1[1] - pos2[1]
            dz = pos1[2] - pos2[2]

            mag = dt * ((dx * dx + dy * dy + dz * dz) ** (-1.5))
            b1m = body1[2] * mag
            b2m = body2[2] * mag

            vel1[0] -= dx * b2m
            vel1[1] -= dy * b2m
            vel1[2] -= dz * b2m

            vel2[0] += dx * b1m
            vel2[1] += dy * b1m
            vel2[2] += dz * b1m

        for (pos, vel, mass) in bodies:
            pos[0] += dt * vel[0]
            pos[1] += dt * vel[1]
            pos[2] += dt * vel[2]


def report_energy(bodies, pairs, e):
    for (body1, body2) in pairs:
        pos1, vel1, mass1 = body1
        pos2, vel2, mass2 = body2
        dx = pos1[0] - pos2[0]
        dy = pos1[1] - pos2[1]
        dz = pos1[2] - pos2[2]
        e -= (mass1 * mass2) / ((dx * dx + dy * dy + dz * dz) ** 0.5)
    for (pos, vel, mass) in bodies:
        e += mass * (vel[0] ** 2 + vel[1] ** 2 + vel[2] ** 2) / 2.
    # print("%.9f" % e)


def offset_momentum(bodies, px, py, pz):
    for (pos, vel, mass) in bodies:
        px -= vel[0] * mass
        py -= vel[1] * mass
        pz -= vel[2] * mass
    (sun_pos, sun_vel, sun_mass) = bodies[0]
    sun_vel[0] = px / sun_mass
    sun_vel[1] = py / sun_mass
    sun_vel[2] = pz / sun_mass


# This function will be called by pyperf. It should not take 'loops' as pyperf handles looping.
# It will use the 'args' from the global scope (parsed in __main__) for iterations.
def bench_nbody_for_pyperf():
    current_iterations = args.iterations
    # reference = 'solarsystem' # Hardcoded

    system = SYSTEM # Use a copy if mutable and modified per run, but this benchmark seems to reset state
    pairs = PAIRS

    offset_momentum(system, 0., 0., 0.)
    report_energy(system, pairs, 0.)
    advance(0.01, current_iterations, system, pairs)
    report_energy(system, pairs, 0.)


if __name__ == '__main__':
    runner = pyperf.Runner()
    runner.metadata['description'] = "n-body benchmark"

    parser = runner.argparser
    parser.add_argument("--iterations",
                        type=int, default=DEFAULT_ITERATIONS,
                        help="Number of nbody advance() iterations "
                             "(default: %s)" % DEFAULT_ITERATIONS)

    # Make args global so bench_nbody_for_pyperf can access it
    # This is a simplification for this specific script context
    global args
    args = runner.parse_args()

    # Reset stats before benchmark
    (0.0).reset_freelist_stats()

    runner.bench_func('nbody', bench_nbody_for_pyperf)

    # Get and print stats after benchmark
    freelist_stats = (0.0).get_freelist_stats()
    print(f"N-Body Benchmark Complete.")
    print(f"Freelist Stats: hits={freelist_stats['hits']}, misses={freelist_stats['misses']}")
