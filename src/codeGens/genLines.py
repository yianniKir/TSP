import itertools

def generate_line_names(num_points):
    point_names = [chr(ord('A') + i) for i in range(num_points)]
    line_names = []
    for combination in itertools.combinations(point_names, 2):
        line_names.append(''.join(combination))
    return line_names

def write_line_names_to_file(line_names, output_file):
    with open(output_file, 'w') as file:
        for line_name in line_names:
            file.write('"' + line_name + '"' + ',\n')

def main():
    num_points = 7  # Number of points
    line_names = generate_line_names(num_points)
    output_file = 'lines.txt'
    write_line_names_to_file(line_names, output_file)
    print(f"Generated line names are written to {output_file}")

if __name__ == "__main__":
    main()
