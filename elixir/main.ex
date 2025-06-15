defmodule Main do
  def main do
    {:ok, _} = Application.ensure_all_started(:exla)
    IO.inspect(Nx.tensor([[1, 2], [3, 4]]))
    {stdout, _} = System.cmd("cat", ["main/mix.lock"])
    IO.puts(stdout)
  end
end
