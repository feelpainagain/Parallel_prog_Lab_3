$matrix_sizes = 100..1000 | Where-Object {$_ % 100 -eq 0}
$exe_path = "C:\Users\kiril\Desktop\Parallel_lab3\cmake-build-debug\Lab_3.exe"  # Укажи путь к своему .exe файлу

foreach ($size in $matrix_sizes) {
    Write-Host "Running size $size..."
    $startTime = Get-Date
    mpiexec -n 4 $exe_path $size
    $endTime = Get-Date
    $duration = $endTime - $startTime
    Write-Host "Size $size finished in $($duration.TotalSeconds) seconds"
}
