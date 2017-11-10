import attr
import string
import random
from random import randint


all_nodes = ["closeToWall", "farFromWall", "doTwoFarFar", "doTwoCloseClose", "doTwoCloseFar", "goForward", "turnRight",
             "turnLeft", "backUp", "turnSquareWithWall"]
random.shuffle(all_nodes)
leaf_nodes = ["goForward", "turnRight", "turnLeft", "backUp", "turnSquareWithWall"]
internal_TwoChildren = ["doTwoFarFar", "doTwoCloseClose", "doTwoCloseFar"]
internal_SingleChild = ["closeToWall", "farFromWall"]
unique_id_list = []

"""
Represents a Node in a tree
"""
@attr.s
class Node:
    name = attr.ib()
    depth = attr.ib(default=0)
    parent_id = attr.ib(default=0)
    id = attr.ib(default=0)
    sensor = attr.ib(default=None)
    child1 = attr.ib(default=None)
    child2 = attr.ib(default=None)
    children = attr.ib(default=attr.Factory(list))


def generateUniqueID(length=12):
    """
    generate a unique id with length 12 consisting of letters and numbers
    """
    unique_id = ""
    in_list = True
    while in_list:
        unique_id = ''.join(random.choices(string.ascii_letters + string.digits, k=length))
        in_list = (unique_id in unique_id_list)

    unique_id_list.append(unique_id)
    return unique_id

def getNumberOfChildren(name=None):
    """
    Returns the number of children it has based on the function name
    """
    if name in internal_TwoChildren:
        return 2
    elif name in internal_SingleChild:
        return 1
    else:
        return 0

def generateChildren(node=None, parent=None, id=0, depth=1, max_depth=9):
    """
    Generates the children for a specific node
    """
    number_children = getNumberOfChildren(node.name)
    if depth < max_depth:
        if number_children == 2:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1,4), depth=depth)
            generateChildren(node=node.child1, depth=depth+1)

            node.child2 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth)
            generateChildren(node=node.child2, depth=depth+1)

        elif number_children == 1:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth)
            generateChildren(node=node.child1, depth=depth+1)
    else:
        if number_children == 2:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth)
            node.child2 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth)
        elif number_children == 1:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth)


def buildTrees():
    """
    Builds the trees by first creating the parent/s
    """
    roots = []
    user_choice = int(input("How many trees do you want? "))
    for number in range(user_choice):
        roots.append(Node(name=random.choice(all_nodes), id=generateUniqueID()))

        if roots[number].name not in leaf_nodes:
            roots[number].sensor = randint(1, 4)

        generateChildren(node=roots[number], id=1)

    return roots


def printTrees(trees=None):
    """
    Print the tree/s
    """
    for node in trees:
        printNode(node=node)


def findNode(node=None, node_id="", not_found=False):
    current = node
    while current is not None:
        if current.id == node_id:
            return current

        if current.child1 is None:
            current = current.child2
        else:
            pre = current.child1
            while pre.child1 is not None and pre.child2 != current:
                pre = pre.child2

            if pre.child2 is None:
                pre.child2 = current
                current = current.child1
            else:
                pre.child2 = None
                current = current.child2
    return current



def editTrees(trees=None):
    node_id = str(input("Please enter the id of the node you'd like to edit: "))
    node = None
    if node_id not in unique_id_list:
        print("Error: not a valid id\n")
    else:
        for root in trees:
            node = findNode(node=root, node_id=node_id)

    if node is not None:
        name = ""
        sensor = 0

        print(all_nodes)
        valid = False
        while not valid:
            name = input("Current name: " + str(node.name) + ".......... Enter new name from the above list of functions: ")
            valid = name in all_nodes

        valid = False
        while not valid:
            sensor = int(input("Current sensor: " + str(node.sensor) + "   .......... Enter new sensor (number must be from 1 to 4): "))
            valid = sensor in [1, 2, 3, 4]

        node.name = name
        node.sensor = sensor
        print("Regenerating list from edited node ......")
        generateChildren(node=node, depth=node.depth+1)
        print("Done.")


def printNode(node=None, depth=0, max_depth=9, tabs=""):
    """
    prints a specific node
    """

    separator = "------------------------------"

    print(tabs + separator)
    print(tabs + "|->name: " + str(node.name))
    print(tabs + "|->id: " + str(node.id))
    print(tabs + "|->sensor: " + str(node.sensor))
    print(tabs + "|->depth: " + str(node.depth))
    print(tabs + separator)

    if node.child1 is not None:
        printNode(node=node.child1, depth=depth+1, tabs=tabs+"\t")

    if node.child2 is not None:
        printNode(node=node.child2, depth=depth+1, tabs=tabs+"\t")

if __name__ == '__main__':
    end = False
    trees = []
    while not end:
        choice = int(input("1. Build tree, 2. Edit tree, 3. Print trees, 4. End "))
        if choice == 1:
            trees = buildTrees()
        elif choice == 2:
            if not trees:
                print("You didn't create a tree yet!!!!\n")
            else:
                editTrees(trees)
        elif choice == 3:
            printTrees(trees)
        elif choice == 4:
            end = True