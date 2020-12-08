- Changing to a candidate list reduced the amount of work that any particular ant does to a O(n). This means that the global update, which is O(n^2) swamps the each ants computation
- This results in not enough work for siginificant speed up in parallizing my ants
- I did manage to get good speedup when parallizing the global update

- Fixes: Implements 3-opt. By doing this the amount of work each ant does will increase, hopefully providing a better parallelization as well as results.