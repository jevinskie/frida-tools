entrypoint = 'agent.ts'

sources = [
  entrypoint,
  'package.json',
  'package-lock.json',
  'tsconfig.json',
]

custom_target('report-exceptions-agent.js',
  input: sources,
  output: ['report_exceptions_agent.js'],
  command: [
    build_agent,
    '@INPUT@',
    '@OUTPUT0@',
    '@PRIVATE_DIR@',
  ],
  install: true,
  install_dir: pkgdir,
)
