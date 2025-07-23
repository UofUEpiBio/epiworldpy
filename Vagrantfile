Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"

  config.vm.provider "virtualbox" do |vb|
    vb.memory = "2048"
    vb.cpus = 2
  end

  config.vm.network "private_network", type: "dhcp"

  config.vm.provision "shell", inline: <<-SHELL
    # Update and install prerequisites
    sudo apt-get update
    sudo apt-get install -y python3 python3-pip python3-venv cmake build-essential

    # Create a Python virtual environment
    python3 -m venv /home/vagrant/epiworldPy-env

    # Activate virtual environment and upgrade pip
    source /home/vagrant/epiworldPy-env/bin/activate
    pip install --upgrade pip setuptools wheel

    # Install dependencies from pyproject.toml (if it exists)
    if [ -f /vagrant/pyproject.toml ]; then
      pip install .
    fi

    # Deactivate the virtual environment
    deactivate
  SHELL

  # Synchronize the project directory with the VM
  config.vm.synced_folder ".", "/vagrant"
end
