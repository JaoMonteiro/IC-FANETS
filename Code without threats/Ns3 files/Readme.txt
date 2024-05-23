João Monteiro Delveaux Silva <joaomont@ufmg.br> - Engenharia de Sistemas UFMG April 2024

Folder "addons" is a custom module built for ns3. It must be placed inside folder "src" of ns3, and then ns3 will automatically build the module on the next simulation or you can run "./build.py" on ns3 root folder.

Folder "application" contains the main application code, which has the simulation configurations. It must be placed inside folder "scratch" of ns3.

Folder "Node Class" contains the extended node class files which must be placed inside folder "src/network/model" and replace the standard node class ones.
