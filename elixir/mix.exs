defmodule Main.MixProject do
  use Mix.Project

  def project do
    [
      app: :main,
      version: "0.1.0",
      elixir: ">= 0.0.0",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  def application do
    [
      extra_applications: [:logger, :mnesia]
    ]
  end

  defp deps do
    [
      {:aja, ">= 0.0.0"},
      {:arrays, ">= 0.0.0"},
      {:bitset, ">= 0.0.0"},
      {:collections, ">= 0.0.0"},
      {:decimal, ">= 0.0.0"},
      {:exla, ">= 0.0.0"},
      {:flow, ">= 0.0.0"},
      {:libgraph, ">= 0.0.0"},
      {:matrex, ">= 0.0.0"},
      {:math, ">= 0.0.0"},
      {:nx, ">= 0.0.0"},
      {:picosat_elixir, ">= 0.0.0"},
      {:prime, ">= 0.0.0"},
      {:ratio, ">= 0.0.0"},
      {:segment_tree, ">= 0.0.0"},
      {:splay_tree, ">= 0.0.0"},
      {:trie, ">= 0.0.0"},
    ]
  end
end
