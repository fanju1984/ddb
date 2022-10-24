from graphviz import Digraph

dot = Digraph('测试', format="svg")
dot.node("1","Life's too short")
dot.node("2","I learn Python")
dot.edge('1','2')

dot.render(filename="test.svg", directory="./", view=False)