int *array_builder(int *arr)
{
  for (int i = 0; i < 20; ++i)
  {
    arr[i] = i * i;
  }

  return arr;
}

int main()
{
  int *arr = new int[20];

  array_builder(arr);

  delete[] arr;

  return 0;
}
