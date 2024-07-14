from PIL import Image, ImageDraw, ImageFont
import csv
import sys
import os
import math
import random

def load_graph_from_csv(filename):
    graph = {}
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip header
        for row in reader:
            source, destination, weight = row
            source = source.strip()
            destination = destination.strip()
            weight = int(weight.strip())
            if source not in graph:
                graph[source] = []
            if destination not in graph:
                graph[destination] = []
            graph[source].append((destination, weight))
            graph[destination].append((source, weight))  # Assuming bidirectional edges
    return graph

def print_graph(graph):
    for node, edges in graph.items():
        print(f"{node}:")
        for edge in edges:
            print(f"  -> {edge[0]} (weight: {edge[1]})")

def dijkstra(graph, start, end):
    import heapq
    queue = [(0, start, [])]
    seen = set()
    mins = {start: 0}
    while queue:
        (cost, node, path) = heapq.heappop(queue)
        if node in seen:
            continue
        seen.add(node)
        path = path + [node]
        if node == end:
            return (cost, path)

        for (next_node, weight) in graph.get(node, []):
            if next_node in seen:
                continue
            prev = mins.get(next_node, None)
            next_cost = cost + weight
            if prev is None or next_cost < prev:
                mins[next_node] = next_cost
                heapq.heappush(queue, (next_cost, next_node, path))
    return (float("inf"), [])

def draw_path(graph, path, output_file='graph.png'):
    # Create a blank image
    width, height = 1200, 800
    image = Image.new('RGB', (width, height), 'white')
    draw = ImageDraw.Draw(image)

    # Automatic positioning of nodes in a circular layout
    num_nodes = len(graph)
    radius = 300
    center_x, center_y = width // 2, height // 2
    angle_step = 2 * math.pi / num_nodes

    positions = {}
    colors = {}

    for i, node in enumerate(graph.keys()):
        angle = i * angle_step
        x = center_x + radius * math.cos(angle)
        y = center_y + radius * math.sin(angle)
        positions[node] = (int(x), int(y))
        colors[node] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

    # Draw edges first
    for source, neighbors in graph.items():
        for destination, weight in neighbors:
            x1, y1 = positions[source]
            x2, y2 = positions[destination]
            draw.line((x1, y1, x2, y2), fill="grey", width=2)

    # Draw highlighted path underneath rectangles
    for i in range(len(path) - 1):
        x1, y1 = positions[path[i]]
        x2, y2 = positions[path[i+1]]
        draw.line((x1, y1, x2, y2), fill="red", width=6)

    # Draw nodes and their labels
    for node, (x, y) in positions.items():
        node_color = colors[node]
        draw.rectangle([x-40, y-20, x+40, y+20], fill=node_color, outline="black")
        draw.text((x-35, y-10), node, fill="black")

    # Draw weights
    for source, neighbors in graph.items():
        for destination, weight in neighbors:
            x1, y1 = positions[source]
            x2, y2 = positions[destination]
            mx, my = (x1 + x2) // 2, (y1 + y2) // 2
            draw.text((mx, my), str(weight), fill="blue")

    image.save(output_file)
    image.show()

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    csv_file = input("Enter the CSV file name (cities.csv): ")
    csv_path = os.path.join(script_dir, csv_file)

    if not os.path.exists(csv_path):
        print(f"Error: The file '{csv_path}' does not exist.")
        sys.exit(1)

    graph = load_graph_from_csv(csv_path)
    
    print("Loaded graph:")
    print_graph(graph)

    start_node = input("Enter the start node: ")
    end_node = input("Enter the end node: ")

    # Compute the shortest path using Dijkstra's algo
    cost, path = dijkstra(graph, start_node, end_node)

    if path:
        print(f"Shortest distance from [{start_node}] to [{end_node}] is {cost}")
        print("PATH:", " --> ".join(path))
        draw_path(graph, path)
    else:
        print(f"No path found from {start_node} to {end_node}")
