def calculate_alignment_rate(sam_file, total_sequences):
    aligned_count = 0

    with open(sam_file, 'r', encoding='utf-16') as f:
        i=0
        for line in f:
            if line.startswith('@'):
                continue  # 跳过头部行
            # print(line);
            # i+=1
            # if i>100:
            #     break
            fields = line.strip().split('\t')
            flag = fields[2]
            # 判断比对是否成功，通常rname字段不为 "*" 表示比对成功
            if flag != '*':
                aligned_count += 1
            total_sequences += 1

    if total_sequences == 0:
        total_sequences = 1
    alignment_rate = (aligned_count / total_sequences) * 100
    return alignment_rate


# 示例用法
sam_file = 'res.sam'
total_sequences = 0 
rate = calculate_alignment_rate(sam_file, total_sequences)
print(f'Mapping rate {rate:.2f}%')