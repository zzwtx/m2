import sys

def process_file(input_path, output_path):
    with open(input_path, 'r') as f_in, open(output_path, 'w') as f_out:
        i = 0
        for line in f_in:
            if i % 4 == 1:
                stripped_line = line.rstrip('\n')  # 保留行末外的其他空格
                parts = stripped_line.split()
                if len(parts) >= 3:
                    try:
                        parts[2] = str(int(parts[2]) + 111714)
                    except ValueError:
                        pass  # 保持原值（处理无效数字）
                new_line = ' '.join(parts)
                f_out.write(new_line + '\n')
            else:
                f_out.write(line)
            i += 1

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"用法：{sys.argv[0]} 输入文件 输出文件")
        sys.exit(1)
    process_file(sys.argv[1], sys.argv[2])