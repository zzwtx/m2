def calculate_alignment_num(sam_file):
    aligned_count = 0
    tot_count = 0
    last_name = ''

    with open(sam_file, 'r', encoding='utf-16') as f:
        quality = 0
        for line in f:
            fields = line.strip().split('\t')
            if fields[0] != last_name:
                if quality >= 10 and quality != 255:
                    aligned_count += 1
                quality = 0
                tot_count += 1
            quality = max(int(fields[11]), quality)
            last_name = fields[0]
        if quality >= 10 and quality != 255:
            aligned_count += 1

    return [aligned_count, tot_count]


# 示例用法
sam_file = "min_wei_rob.paf"
num = calculate_alignment_num(sam_file)
print(f'High-quality mapping number {num[0]}')
print(f'Mapping number {num[1]}')
print(f'{(num[1]-num[0])/num[1]}')