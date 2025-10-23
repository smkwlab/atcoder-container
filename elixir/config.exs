import Config
config :nx, :default_backend, EXLA.Backend
config :logger, level: :error
# Limit EXLA build parallelism to avoid OOM (default: System.schedulers_online() - 2)
config :exla, :make_args, ["-j4"]
