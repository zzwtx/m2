# 统计总读段数（FASTQ文件）
total_count=$(grep -c "NC" sd1.fq)

echo "总读段数: $total_count"