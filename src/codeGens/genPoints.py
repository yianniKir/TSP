import csv

def generate_point_name(index):
    return chr(ord('A') + index)

def main():
    input_file = 'points.csv'
    output_file = 'points.txt'

    with open(input_file, newline='') as csvfile, open(output_file, 'w') as txtfile:
        csv_reader = csv.reader(csvfile, delimiter=',')
        point_index = 0
        for row in csv_reader:
            point_name = generate_point_name(point_index)
            point_data = f"{point_name.strip()},{row[0].strip()},{row[1].strip()},\n"
            txtfile.write(point_data)
            point_index += 1

    print(f"Generated {point_index} points, written to {output_file}")

if __name__ == "__main__":
    main()
