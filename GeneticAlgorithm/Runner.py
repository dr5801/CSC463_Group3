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

@attr.s
class Node:
    """
    Represents a Node in a tree
    """
    name = attr.ib()
    depth = attr.ib(default=0)
    id = attr.ib(default=0)
    sensor = attr.ib(default=None)
    child1 = attr.ib(default=None)
    child2 = attr.ib(default=None)
    parent = attr.ib(default=None)


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


def reorderDepths(root=None, depth=0, max_depth=9):
    if depth <= max_depth:
        if root is not None:
            if root.depth != depth:
                root.depth = depth
            reorderDepths(root=root.child1, depth=depth+1)
            reorderDepths(root=root.child2, depth=depth+1)


def generateChildren(node=None, parent=None, id=0, depth=1, max_depth=9):
    """
    Generates the children for a specific node
    """
    number_children = getNumberOfChildren(node.name)
    if depth < max_depth:
        if number_children == 2:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1,4), depth=depth, parent=node)
            generateChildren(node=node.child1, depth=depth+1)

            node.child2 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth, parent=node)
            generateChildren(node=node.child2, depth=depth+1)

        elif number_children == 1:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth, parent=node)
            generateChildren(node=node.child1, depth=depth+1)
        else:
            node.child1 = None
            node.child2 = None
    else:
        if number_children == 2:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth, parent=node)
            node.child2 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth, parent=node)
        elif number_children == 1:
            node.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), depth=depth, parent=node)
        else:
            node.child1 = None
            node.child2 = None


def buildTrees():
    """
    Builds the trees by first creating the parent/s
    """
    roots = []
    user_choice = int(input("How many trees do you want? "))
    for number in range(user_choice):
        specialRoot = Node(name="SpecialRoot", id=generateUniqueID())
        specialRoot.child1 = Node(name=random.choice(all_nodes), id=generateUniqueID(), sensor=randint(1, 4), parent=specialRoot)
        roots.append(specialRoot.child1)

        if roots[number].name not in leaf_nodes:
            roots[number].sensor = randint(1, 4)

        generateChildren(node=roots[number], id=1)
    return roots

def redirectTrees(trees=None):
    valid = False
    tree_ids = []
    node1=None
    node2=None

    for i in range(2):
        valid = False
        while not valid:
            if i == 0:
                tree_id = int(input("Please enter the first tree number: "))
            else:
                tree_id = int(input("Please enter the second tree number: "))

            if tree_id not in tree_ids:
                if tree_id <= (len(trees) - 1):
                    node_id = str(input("Please enter the node id for that tree: "))
                    node = findNode(root=trees[tree_id], node_id=node_id)
                    if node is not None:
                        if node1 is None:
                            node1 = node
                        else:
                            node2 = node

                        tree_ids.append(tree_id)
                        valid = True
                    else:
                        print("Could not find node. Trying again... ")
                else:
                    print("Tree id: " + str(tree_id) + " does not exist. Trying again.... ")
            else:
                print("You have to enter a different tree id than the first one. Trying again... ")

    print("hello")
    randomNode = node1.parent
    randomNode2 = node2.parent

    if node1.parent.child1.id == node1.id:
        if node2.parent.child1.id == node2.id:
            node1.parent.child1, node2.parent.child1 = node2.parent.child1, node1.parent.child1
        else:
            node1.parent.child1, node2.parent.child2 = node2.parent.child2, node1.parent.child1
    else:
        if node2.parent.child1.id == node2.id:
            node1.parent.child2, node2.parent.child1 = node2.parent.child1, node1.parent.child2
        else:
            node1.parent.child2, node2.parent.child2 = node2.parent.child2, node1.parent.child2

    node1.parent, node2.parent = node2.parent, node1.parent

    print ("hello")




def findNode(root=None, node_id=""):
    current = root
    s = []
    done = False
    node = None
    while not done:
        if current is not None:
            if current.id == node_id:
                node = current
                done = True
            else:
                s.append(current)
                current = current.child1
        else:
            if len(s) > 0:
                current = s.pop()
                current = current.child2
            else:
                done = True
    return node


def editTrees(trees=None):
    node_id = str(input("Please enter the id of the node you'd like to edit: "))
    node = None
    if node_id not in unique_id_list:
        print("Error: not a valid id\n")
    else:
        for root in trees:
            node = findNode(root=root, node_id=node_id)
            if node is not None:
                break

    if node is not None:
        name = ""
        sensor = 0

        print(all_nodes)
        valid = False
        while not valid:
            name = input("Current name: " + str(node.name) + ".......... Enter new name from the above list of functions: ")
            valid = name in all_nodes

        if name not in leaf_nodes:
            valid = False
            while not valid:
                sensor = int(input("Current sensor: " + str(node.sensor) + "   .......... Enter new sensor (number must be from 1 to 4): "))
                valid = sensor in [1, 2, 3, 4]
        else:
            sensor = 0

        node.name = name
        node.sensor = sensor
        print("Regenerating list from edited node ......")
        generateChildren(node=node, depth=node.depth+1)
        print("Done.")


def printTrees(trees=None):
    """
    Print the tree/s
    """
    i=0
    for node in trees:
        print("\nTree: " + str(i))
        printNode(node=node)
        i+=1
    return node


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
    print(tabs + "|->parent: " + str(node.parent.name))
    print(tabs + separator)

    if node.child1 is not None:
        printNode(node=node.child1, depth=depth+1, tabs=tabs+"\t")

    if node.child2 is not None:
        printNode(node=node.child2, depth=depth+1, tabs=tabs+"\t")

if __name__ == '__main__':
    end = False
    trees = []
    while not end:
        choice = int(input("1. Build tree, 2. Edit tree, 3. Print trees, 4. Redirect trees, 5. End "))
        if choice == 1:
            trees = buildTrees()
        elif choice == 2:
            if not trees:
                print("You didn't create a tree yet!!!!\n")
            else:
                editTrees(trees)
        elif choice == 3:
            printTrees(trees=trees)
        elif choice == 4:
            if len(trees) <= 1:
                print("You need to build at least 2 trees. (1.Build tree, build 2 or more trees, then come back ;)")
            else:
                redirectTrees(trees=trees)
                for root in trees:
                    reorderDepths(root=root, depth=0)
        elif choice == 5:
            end = True