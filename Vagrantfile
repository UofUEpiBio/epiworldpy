Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"

  config.vm.provider "virtualbox" do |vb|
    vb.memory = "2048"
    vb.cpus = 2
  end

  config.vm.network "private_network", type: "dhcp"

  config.vm.provision "shell", inline: <<-SHELL
    # Update and install prerequisites
    apt-get update -y
    apt-get install -y python3 python3-pip python3-venv build-essential cmake git curl

    # Install uv (standalone binary install is easiest)
    curl -LsSf https://astral.sh/uv/install.sh | sh
    export PATH="$HOME/.cargo/bin:$PATH"

    # Create and activate a uv environment for the project
    cd /vagrant
    uv venv .venv
    source .venv/bin/activate

    # Install editable package
    uv pip install -e .
  SHELL

  # Synchronize the project directory with the VM
  config.vm.synced_folder ".", "/vagrant"
end
