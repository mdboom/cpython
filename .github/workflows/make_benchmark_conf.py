import os
import sys

sys.stdout.write(sys.stdin.read().format(**os.environ))
